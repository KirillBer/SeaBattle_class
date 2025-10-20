#include <iostream>
#include <cstdlib>	
#include <locale>	
//#include "Field.h"	//���������� "���� ���� '������� ���'"
#include <vector>	//��� ���������� � ������ �����
#include <cstring>	//��� ������ � �������
#include <fstream>	//��� ������ � �������
#include <iomanip> //��� �������������� ������
#include <windows.h> // ��� ��������� ����� ������
#include <time.h> // ��� ������������� ���������� ��������� �����

using namespace std;

class SeaBattleField;


class SeaBattleField{	//�������� � ������ ���� � ��������
		//�����������: EMPTY, SHOT, STRIKE, KILL, SHIP
	private:	//��������
		struct CellValueChange{	//�������� ��������� ��������� ������ ��� ����
		    int coordinate_index;	//������ ������
		    unsigned char prevState;	//��������� ������ �� ���������
		    unsigned char newState;		//��������� ������ ����� ���������
		};
		int cols, rows;	//��������� �������� �� 1 �� 100 ���������, �� cols * rows �� 1 ���������
		unsigned char *field;	//��������� �������� �� 0 �� 255
		unsigned char *field_value_mean;	//��������� �������� �� 0 �� 255
		unsigned char *ships;	//ships[0] - ������������ ����� �������, ships[�� 1 �� 255] - ���-�� ��������. ����. ���-�� �������� ����� ����� - 255
		unsigned char *ships_remain;	//����� ships, ������ ships_remain �������� ���������� ���-��� �������� � ����, � �� ���������� ����
	
		vector<vector<CellValueChange> > moves;	//�������� �����
		
	
		//������������ � ���������� ����������
	public:
		SeaBattleField(int cols_ = 10, int rows_ = 10): cols(cols_), rows(rows_){
			field = 0;
			field_value_mean = 0;
			ships = 0;
			ships_remain = 0;
			if (IsNewSizeAvailable(cols_, rows_))
			{
				cols = 10;
				rows = 10;
			}
			field = new unsigned char[cols * rows];
			field_value_mean = new unsigned char[5];
			ships = new unsigned char[5];
			ships_remain = new unsigned char[5];
			if (!field || !field_value_mean || !ships || !ships_remain)
				MemoryAllocationError(0);
			ClearField();
			ResetFieldValueMean();
			ResetShips();
			ResetShipsRemain();
		}
		~SeaBattleField(){
			printf("�����.\n");
			if (field)
				delete[] field;
			if (field_value_mean)
				delete[] field_value_mean;
			if (ships)
				delete[] ships;
			if (ships_remain)
				delete[] ships_remain;
			printf("����� ���������.\n");
		}
		SeaBattleField(const SeaBattleField& other){
			printf("������ ����������� �����������\n\n");
			field = 0;
			field_value_mean = 0;
			ships = 0;
			ships_remain = 0;
			cols = 1;
			rows = 1;
			printf("cols � ������ = %d, rows = %d\n", cols, rows);
			field = new unsigned char[1];
			field_value_mean = new unsigned char[5];
			ships = new unsigned char[1];
			ships_remain = new unsigned char[1];
			if (!field || !field_value_mean || !ships || !ships_remain)
				MemoryAllocationError(0);
			/*������������� ��� �������� "�� ���������"
			ClearField();
			ResetFieldValueMean();
			ResetShips();
			ResetShipsRemain();
			*/
			printf("���\n");
			printf("other.cols = %d, %p\n\n\n", other.cols, &other.cols);
			printf("this fvm0 = %d %d\n", field_value_mean[0], field_value_mean[1]);
				printf("111\n");
	        printf("���� ChangeFieldSize: %d\n", ChangeFieldSize(other.cols, other.rows));
			ChangeFieldValueMean(other.field_value_mean[0], other.field_value_mean[1], other.field_value_mean[2], other.field_value_mean[3], other.field_value_mean[4]);
				printf("222\n");
	        for(int i = cols * rows - 1; i >= 0; i--)
	            field[i] = other.field[i];
	            printf("333\n");
	        printf("���� ChangeMaxShipLen: %d\n", ChangeMaxShipLen(other.ships[0]));
	            printf("444\n");
			for(int i = 1; i <= other.ships[0]; i++)
				printf("���� ChangeShipLenCount: %d\n", ChangeShipLenCount(i, other.ships[i]));
				printf("555\n");
			moves = other.moves;
			printf("cols = %d, rows = %d\n", cols, rows);
			for(int i = 0; i < rows; i++, printf("\n"))
				for(int n = 0; n < cols; n++)
					printf("%d ", field[cols * i + n]);
			printf("����� ����������� �����������\n\n");
		}
		SeaBattleField &operator=(const SeaBattleField& other){
	        printf("������ �������� ������������\n\n");
			if (this != &other) {
	            ChangeFieldValueMean(other.field_value_mean[0], other.field_value_mean[1], other.field_value_mean[2], other.field_value_mean[3], other.field_value_mean[4]);
	            printf("1\n");
				printf("���� ChangeFieldSize: %d\n", ChangeFieldSize(other.cols, other.rows));
	            printf("2\n");
				for(int i = cols * rows - 1; i >= 0; i--)
	            	field[i] = other.field[i];
	            printf("3\n");
	            printf("���� ChangeMaxShipLen: %d\n", ChangeMaxShipLen(other.ships[0]));
	            printf("4\n");
				for(int i = 1; i <= other.ships[0]; i++)
					printf("���� ChangeShipLenCount: %d\n", ChangeShipLenCount(i, other.ships[i]));
				printf("5\n");
				moves = other.moves;
				printf("cols = %d, rows = %d\n", cols, rows);
				for(int i = 0; i < rows; i++, printf("\n"))
					for(int n = 0; n < cols; n++)
						printf("%d ", field[cols * i + n]);
				
	            // �������� ������ �� ������� �������
	        }
	        printf("����� �������� ������������\n\n");
	        return *this; // ���������� ������� ������
	    }
		
		
		//��������� ���������� ��������
	public:
		int GetRows(){	//���������� �������� rows
			return rows;
		}
		int GetCols(){	//���������� �������� cols
			return cols;
		}
		const int GetValueOfValueMean(int type_position) const{	//���� type_position[0, 4] - ������� ������� �������� ����, ����� ������� 1111 (����������� ��������)
			if (type_position < 0 || type_position > 4)
				return 1111;
			return field_value_mean[type_position];
		}
		int GetShipsRemainCountOfNLen(int n){	//[1, (ship_max_len - 1)] - ���������� ��������; -1 - ������ ����� �������
			if (n < 1 || n > ships[0])
				return -1;
			return ships_remain[n]; // ������� ���-�� ���������� �������� ����� n
		}
		int GetMaxShipLen(){	//���������� ����� ������ �������� �������
			return ships[0];
		}
		int GetCountOfShips(){	//���������� ����� ��������, ������� ����� � ����
			int sum = 0;
			for(int i = 1; i <= ships[0]; i++)
				sum += ships[i];
			return sum;
		}
		int GetCountOfShipsRemain(){	//���������� ����� ���������� �������� � ����
			int sum = 0;
			for(int i = 1; i <= ships_remain[0]; i++)
				sum += ships_remain[i];
			return sum;
		}
		int GetCellsOfShipsSum(){	//���������� ����� ������ �������� � ������� ����� � ����
			int sum = 0;
			for(int i = 1; i <= ships[0]; i++)
				sum += (ships[i] * i);
			return sum;
		}
		int GetCellsOfShipsRemainSum(){	//���������� ����� ������ �� ���������� ��������, ���������� � ����
			int sum = 0;
			for(int i = cols * rows - 1; i >= 0; i--)
				if (field[i] == field_value_mean[4] || field[i] == field_value_mean[2])
					sum += 1;
			return sum;
		}
		const int LookAtCellByIndex(int index) const{	//���������� ���������� �������� � ������ (��� ������ � ����� ������� ���������� �������� ������ ������), ����� -1
			if (index < 0 || index >= rows * cols)
				return -1;
			if (field[index] == field_value_mean[4])
				return field_value_mean[0];
			return field[index];
		}
	protected:
		int GetValueOfCellByIndex(int index){	//���������� ���������� �������� � ������, ����� -1
			if (index < 0 || index >= rows * cols)
				return -1;
			return field[index];
		}
		int GetValueOfCell(int col_index, int row_index){	//���������� ���������� �������� � ������, ����� -1
			if ((col_index < 0 || row_index < 0) || (col_index >= cols || row_index >= rows))
				return -1;
			return field[row_index * rows + col_index];
		}
		
		
		//������ �������� �� �����������
	public:
		void ClearField(){	//������ ������ ��������� �������� EMPTY ����� ����
			for(int i = cols * rows - 1; i >= 0; i--)
				field[i] = field_value_mean[0];
		}
		void ResetFieldValueMean(){	//�������� ����� ������ ���� ���������� ������������ (EMPTY - 0, SHOT - 1, ..., SHIP - 4)
			for(int i = 0; i < 5; i++)
				field_value_mean[i] = i;
		}
		void ResetShips(){	//���������� �������� ���������� ����� 4 1-��������, ..., 1 4-��������
			unsigned char *t = 0;
			t = new unsigned char[5];
			if (!t)
				MemoryAllocationError(1);
			else
			{
				if (ships)
					delete[] ships;
				ships = t;
				ships[0] = 4;	//����. �����
				ships[1] = 4;
				ships[2] = 3;
				ships[3] = 2;
				ships[4] = 1;
			}
		}
		void ResetShipsRemain(){	//�������� ships_remain ���������� ����� ��������� ships
			for(int i = 0; i <= ships[0]; i++)
				ships_remain[i] = ships[i];
		}
		void ResetField(){	//������� ��� ��������� ����
			ClearField();
			while(moves.size() > 0)
				moves.pop_back();
		}
		
		//��������� �������� ��� ����
	public:
		int ChangeFieldValueMean(int EMPTY = 0, int SHOT = 1, int STRIKE = 2, int KILL = 3, int SHIP = 4){	//0 - �������; 1 - ������, ���������� ��������; 2 - ������������ �������� (<0 ��� >255)
			if (IsInCharRange(EMPTY) || IsInCharRange(SHOT) || IsInCharRange(STRIKE) || IsInCharRange(KILL) || IsInCharRange(SHIP))
				return 2;
			char temp[5] = {(char)EMPTY, (char)SHOT, (char)STRIKE, (char)KILL, (char)SHIP};
			for(int i = 0; i < 5; i++)	//�����������, ���� 2 ������ ���� ������ ���� ��������� ���������
				for(int n = 0; n < 5; n++)
					if (i != n)
						if (temp[i] == temp[n])
							return 1;
			for(int i = cols * rows - 1; i >= 0; i--)	//������ ������ �������� ����� � ������� �� �����
				if (field[i] == field_value_mean[0])
					field[i] = EMPTY;
				else if (field[i] == field_value_mean[1])
					field[i] = SHOT;
				else if (field[i] == field_value_mean[2])
					field[i] = STRIKE;
				else if (field[i] == field_value_mean[3])
					field[i] = KILL;
				else if (field[i] == field_value_mean[4])
					field[i] = SHIP;
			field_value_mean[0] = EMPTY;
			field_value_mean[1] = SHOT;
			field_value_mean[2] = STRIKE;
			field_value_mean[3] = KILL;
			field_value_mean[4] = SHIP;
			return 0;
		}
		int ChangeFieldSize(int new_cols_count, int new_rows_count){	//0 - �������; 1 - ������ ��������� ������; 2 - ������������ ������� ����
			if (IsNewSizeAvailable(new_cols_count, new_rows_count))
				return 2;
			if ((new_cols_count == cols) && (new_rows_count == rows))
				return 0;
			unsigned char *t = new unsigned char[new_cols_count * new_rows_count];
			if (!t)
				return MemoryAllocationError(1);
			if (field)
				delete[] field;
			field = t;
			cols = new_cols_count;
			rows = new_rows_count;
			ClearField();
			return 0;
		}
		int ChangeShipLenCount(int ship_len, int new_ship_count){	//�������� �������� ��������� �������� ����������� �����. 0 - �������; 1 - ������ ��������
			if ((ship_len < 1 || ship_len > ships[0]) || (new_ship_count < 0 || new_ship_count > 255))
				return 1;
			ships[ship_len] = new_ship_count;
			ships_remain[ship_len] = new_ship_count;
			return 0;
		}
		int ChangeMaxShipLen(int new_max_len){	//�������� ������������ ��������� ����� �������. 0 - �������; 1 - ������ ��������� ������; 2 - ������������ ������������ �����
			if (new_max_len < 1 || new_max_len > 255)
				return 1;
			unsigned char *temp = 0;
			temp = new unsigned char[new_max_len + 1];
			if (!temp)
				return MemoryAllocationError(1);
			temp[0] = new_max_len;
			for(int i = 1; i <= temp[0]; i++)	//����������� � ����� ������ ������ � �������� � ������ � ����� ������ 0
				if (i <= ships[0])
					temp[i] = ships[i];
				else
					temp[i] = 0;
			if (ships_remain)
				delete[] ships_remain;
			ships_remain = temp;
			temp = 0;
			temp = new unsigned char[new_max_len + 1];
			if (!temp)
				return MemoryAllocationError(1);
			for(int i = 0; i <= ships_remain[0]; i++)
				temp[i] = ships_remain[i];
			if (ships)
				delete[] ships;
			ships = temp;
			return 0;
		}
		
		
		//������ � ������������ ������
	private:
		void MakeMove(){	//������ ����� ���
			vector<CellValueChange> move;
			moves.push_back(move);
		}
		void AddCellValueChange(int index, unsigned char prev_value, unsigned char new_value){	//��������� � ��������� ��� ��������� ������
			CellValueChange change = {index, prev_value, new_value};
        	moves[moves.size() - 1].push_back(change);
		}
		void TransformByIndexValueChange(int index, unsigned char prev_value, unsigned char new_value){	//���� � ��������� ���� �� index ������ � ������ prev � new �������� �� �����
			int i = 0, moves_index = (moves.size() - 1);
			int move_index = (moves[moves_index].size() - 1);
			while(index != moves[moves_index][i].coordinate_index && i <= move_index)
				i++;
			if (!(i > move_index))
			{
				moves[moves_index][i].prevState = prev_value;
				moves[moves_index][i].newState = new_value;
			}
		}
	public:
		int CancelLastMove(){	//���������� ��������� �� ��������� ���; ����������: 0 - ���� �����������; 1 - ��� ������� �� SHOT, STRIKE, KILL; 2 - ���� ��������� � SHIP; 3 - ��� ������ �������; 4 - ��� ������� �� EMPTY; 5 - ���� ��������� �������;
			int result = 4;
			if (moves.size())
			{
				if (!(moves[moves.size() - 1].size()))
				{
					moves.pop_back();
					return 1;
				}
				for (int index, i = (moves[moves.size() - 1].size() - 1); i >= 0; i--)
	        	{
	        		index = moves[moves.size() - 1][i].coordinate_index;
	        		if (moves[moves.size() - 1][i].newState == field_value_mean[4])	//����� SHIP
	        			result = 5;
					else if (moves[moves.size() - 1][i].newState == field_value_mean[2])	//����� STRIKE
						result = 2;
					else if (moves[moves.size() - 1][i].newState == field_value_mean[3])	//���� SHIP
					{
	        			int x = (index % cols), y = (index / cols), side = FindShipEdge(&x, &y), len = CalculateWhatInSide(x, y, side, -1);
						ships_remain[len] += 1;
						result = 3;
					}
	        		field[index] = moves[moves.size() - 1][i].prevState;
				}
				moves.pop_back();
			}
			else
				return 0;
			return result;
		}
		const int CheckLastMove() const{ //"�����������", ��� ����� ��������� ���; ���������� �� ��, ��� � CancelLastMove()
			if (!moves.size())
				return 0;
			if (!moves[moves.size() - 1].size())
				return 1;
			if (moves[moves.size() - 1][0].newState == field_value_mean[4])
	        	return 5;
			else if (moves[moves.size() - 1][0].newState == field_value_mean[2])
				return 2;
			else if (moves[moves.size() - 1][0].newState == field_value_mean[3])
				return 3;
			else
				return 4;
		}
		
		
		//������ �� �����
	public:
		static int LoadGameSBF(SeaBattleField &field_1, SeaBattleField &field_2, const char *file_name){	//�������� �� �����; �� ��, ��� � � LoadFromFile
			ifstream file(file_name);	//��������� ���� ��� ������
			int t1 = field_1.LoadFromFile(file);
			if (t1)
				return t1;
			int t2 = field_2.LoadFromFile(file);
			if (t2)
				return t2;
			return 0;
		}
	private:
		int LoadFromFile(ifstream &read_file){	//�������� �� �����; 0 - �������; 1 - ������ �������� ����� / ��������� ������; 2 - ������ ���������� ������; 3 - ������ �������� ������ � �����
			//ifstream read_file(char_file_name);	//��������� ���� ��� ������
			if (read_file.is_open())
			{
		        int a, b;
		        
		        //���������� cols � rows
			    if (!(read_file >> a) || !(read_file >> b))           
					return ReadFileError(read_file, 2);
				
				if (ReadIsStringEnd(read_file) || ChangeFieldSize(a, b))
					return ReadFileError(read_file, 3);
				
				//���������� �������� field
				a = ReadArray(read_file, field, 0, 0, cols * rows);
				if (a)
					return ReadFileError(read_file, a);
					
				//���������� �������� field_value_mean
				if (!(read_file.get() == 'f'))
					return ReadFileError(read_file, 2);
				printf("  field_value_mean\n");
				a = ReadArray(read_file, field_value_mean, 0, 0, 5);
				if (a)
					return ReadFileError(read_file, a);
					
				//���������� �������� ships
				if (!(read_file.get() == 's'))
					return ReadFileError(read_file, 2);
				if (ReadICRNumber(read_file, &a))
					return ReadFileError(read_file, a);
				ships[0] = a;
				if (ChangeMaxShipLen(a))
					return ReadFileError(read_file, 3);
				printf("  ships\n");
				a = ReadArray(read_file, ships, 0, 1, a + 1);
				if (a)
					return ReadFileError(read_file, a);
					
				//���������� �������� ships_remain
				if (!(read_file.get() == 's'))
					return ReadFileError(read_file, 2);
				if (ReadICRNumber(read_file, &a))
					return ReadFileError(read_file, a);
				ships_remain[0] = a;
				if (ships[0] != ships_remain[0])
					return ReadFileError(read_file, 3);
				printf("  ships_remain\n");
				a = ReadArray(read_file, ships_remain, 0, 1, a + 1);
				if (a)
					return ReadFileError(read_file, a);
				for(int i = 0; i <= ships[0]; i++)
					if (ships[i] < ships_remain[i])
						return ReadFileError(read_file, 3);
				//���������� �������� �����
				if (!(read_file >> b))           
					return ReadFileError(read_file, 2);
				if (ReadIsStringEnd(read_file))
					return ReadFileError(read_file, 3);
				for(int i = 0; i < b; i++)
				{
					printf("��� %d\n", i);
					if (!(read_file.get() == 'm') || !(read_file >> a))
						return ReadFileError(read_file, 2);
					a = ReadMove(read_file, a);
					if (a)
						return ReadFileError(read_file, a);
				}
				printf("������ ��������.\n ������ ����\n");
					//�������� ����������� ������������� ������ �������� ����
				//1) ���������� �� �� ���� ���������� ��������
				printf("444\n");
				unsigned char *temp_ships = new unsigned char[ships[0] + 1];
				unsigned char *temp_field = new unsigned char[cols * rows];
				printf("555\n");
				if (!temp_ships || !temp_field)
				{
					if (temp_ships)
						delete[] temp_ships;
					if (temp_field)
						delete[] temp_ships;
					return MemoryAllocationError(1);
				}
				printf("1\n");
				temp_ships[0] = ships[0];
				a = 0;	//������ �������� �� ����
				b = 0;	//����� ������ �������� �� ����
				for(int i = cols * rows - 1, side, x, y; i >= 0; i--)
				{
					temp_field[i] = field_value_mean[0];
					if (field[i] == field_value_mean[4] || field[i] == field_value_mean[3] || field[i] == field_value_mean[2])	//SHIP KILL STRIKE
					{
						if (field[i] == field_value_mean[4])
							b++;
						a++;
						x = i % cols;
						y = i / cols;
						side = FindShipEdge(&x, &y);
						temp_ships[CalculateWhatInSide(x, y, side, -1)]++;
					}
					else
						if (IsOneOfFieldValueMean(field[i]))
							return ReadFileError(read_file, 3);
				}
				printf("2\n");
				if (b != GetCellsOfShipsRemainSum())
					return ReadFileError(read_file, 3);
				for(int i = 1; i <= ships[0]; i++)
				{
					temp_ships[i] = temp_ships[i] / i;
					if (temp_ships[i] != ships[i])
					{
						delete[] temp_ships;
						return ReadFileError(read_file, 3);
					}
				}
				printf("3\n");
				delete[] temp_ships;
				if (a != GetCellsOfShipsSum())
				{
					printf("��������: %d ��������, � ����: %d\n", a, GetCellsOfShipsSum());
					return ReadFileError(read_file, 3);
				}
				printf(" ������ ����\n");
				//2) ������������� �� ���� ����, �� ��� ����������� field
				for(int i = 0; i < moves.size(); i++)	//����� ���� ���������� �� �����
					for(int n = 0; n < moves[i].size(); n++)
					{
						printf("i = %d, n = %d; index = %d, pr_s = %d, ne_s = %d\n", i, n, moves[i][n].coordinate_index, moves[i][n].prevState, moves[i][n].newState);
						if (temp_field[moves[i][n].coordinate_index] == moves[i][n].prevState)
							temp_field[moves[i][n].coordinate_index] = moves[i][n].newState;
						else
						{
							delete[] temp_field;
							return ReadFileError(read_file, 3);
						}
					}
				for(int i = cols * rows - 1; i >= 0; i--)	//��������� ������������ � ���������������� �� ����� �����
					if (field[i] != temp_field[i])
					{
						printf("ERROR: index = %d, fi[] = %d, te[] = %d\n", i, field[i], temp_field[i]);
						delete[] temp_field;
						return ReadFileError(read_file, 3);
					}
				delete[] temp_field;
				printf("  �������� �������\n\n\n");
		    }
		    read_file.close();	// ��������� ����
		}
		int ReadFileError(ifstream &flow_name, int error_type){	//��������� ����� � ���������� ����� ������
			flow_name.close();
			printf("ReadFileError(code = %d)!\n", error_type);
			return error_type;
		}
		int ReadIsStringEnd(ifstream &flow_name){	//��������� �� ��������� ������ � ��������� �� ���������; 0 - ������ �����������; 1 - ������ �� �����������
			if (flow_name.get() == '\n')
				return 0;
			return 1;
		}
		int ReadICRNumber(ifstream &flow_name, int *number){	//���������� �� ����� �����; 0 - �������; 1 - ������ ����������; 2 - ������, �� ��������� �������� char
			if (!(flow_name >> *number))
			{
				*number = 2;
				return 1;
			}
			if (IsInCharRange(*number))
			{
				*number = 3;
				return 2;
			}
			return 0;
		}
		int ReadArray(ifstream &flow_name, unsigned char *array, int add_to_index, int start_from, int max_elements){	//������ �������� �� ����� � ������; 0 - �������; 1 - ������ ��������� �����; 2 - ������ ���������� ������; 3 - ������ �������� ������ � �����
			printf("ReadArray\n");
			for(int a; (!flow_name.eof() && start_from < max_elements); start_from++)
			{
				if (ReadICRNumber(flow_name, &a))
					return a;
				printf("index = %d, a = %d\n", add_to_index + start_from, a);
				array[add_to_index + start_from] = a;
			}
			if (ReadIsStringEnd(flow_name))
			{
				printf("������ �� �����������, ������.\n");
				return 2;
			}
			if (start_from < max_elements)
				return 1;
			return 0;
		}
		int ReadMove(ifstream &flow_name, int size){	//���������� �� ����� ������ ����; 0 - �������; 1 - ������ ��������� �����; 2 - ������ ���������� ������; 3 - ������ �������� ������ � �����
			MakeMove();
			for(int i = 0, index, pr_st, ne_st, index_difference = 1; (!(flow_name.eof()) && i < size); i++)
			{
				if (!(flow_name >> index))
					return 2;
				else
					if (index < 0 || index >= cols * rows)
							return 3;
				if (!(flow_name >> pr_st) || !(flow_name >> ne_st))
					return 2;
				if (i > 0 && (pr_st == field_value_mean[0] && ne_st == field_value_mean[4]))	//�������� �� ������� ����������� ������
				{
					index_difference = moves[moves.size() - 1][i - 1].coordinate_index - index;	
					if ((index_difference != 1 && index_difference != -1) && (index_difference != cols && index_difference != -cols))
						return 3;
				}
				if (pr_st == field_value_mean[1])
					return 3;
				if ((pr_st == ne_st) || (IsOneOfFieldValueMean(pr_st) || IsOneOfFieldValueMean(ne_st)))
					return 3;
				AddCellValueChange(index, pr_st, ne_st);
			}
			if (ReadIsStringEnd(flow_name))
			{
				printf("������ �� �����������, ������.\n");
				return 2;
			}
			if (flow_name.eof())
				return 1;
			return 0;
		}
		
		//������ � ����
	public:
		/*
		static int SaveGameSBF(SeaBattleField &field_1, SeaBattleField &field_2, const char *file_name){	//���������� � ����; �� ��, ��� � � SaveToFile
			ofstream file(file_name);	//��������� ���� ��� ������
			field_1.SaveToFile(file);
			field_2.SaveToFile(file);
		}
		*/
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		bool HaveFormatInName(string file_name, string file_format = ".txt"){
			if (file_name.length() < 4)
				return false;
			return file_name.compare(file_name.length() - 4, 4, file_format) == 0;
		}
		
		
		
		int SaveToFile(string file_name, bool append = false){	//���������� � ����; 0 - �������; 1 - ������������ ��������/������ �������� �����; 2 - ������ ������
			if (file_name.length() < 3)
				return 1;
			if (!HaveFormatInName(file_name, ".txt"))
				file_name += ".txt";
			/*
			//�������� ����� ������ ��������:
			std::ios::in        // ��� ������
			std::ios::out       // ��� ������  
			std::ios::binary    // �������� �����
			std::ios::trunc     // ����� ���� �� ���� (����������)
			std::ios::app       // ���������� � ����� (append)
			std::ios::ate       // ������� � ������� � ����� �����
			
			//����������:
			std::ios::out | std::ios::trunc    // ���������� (�� ��������� ��� ofstream)
			std::ios::out | std::ios::app      // ���������� � �����
			std::ios::in | std::ios::out       // ������ � ������
			*/
			
			ofstream file(file_name.c_str(), (append ? ios::app : ios::trunc));	//��������� ���� ��� ������
			if (!file.is_open())
				return 1;
			cout << "������ ������\n";
			if	(
				!RecordString(file, (append ? "SeaBattleField2\n" : "SeaBattleField1\n")) ||
				!RecordElement(file, "c", cols) ||
				!RecordElement(file, "r", rows) ||
				!RecordArray(file, "f", field, cols * rows) ||
				!RecordArray(file, "fvm", field_value_mean, 5) ||
				!RecordArray(file, "s", ships, ships[0] + 1) ||
				!RecordArray(file, "sr", ships_remain, ships_remain[0] + 1) ||
				!RecordMoves(file)
				){	//������ ������
				file.clear();	//�������� ��������� ������
				cout << "������ ������\n";
			}
			else	//�������� ������
				cout << "�������� ������\n";
			return 0;
			
		}
	private:
		bool RecordString(ofstream &flow_name, string your_string){	//�������� � ���� ������; true - �������; false - ������ ������
			//cout << "RecordString\n";
			return (flow_name << your_string).good();
		}
		bool RecordNumber(ofstream &flow_name, int number){	//�������� � ���� �����; true - �������; false - ������ ������
			//cout << "RecordNumber\n";
			return (flow_name << number).good();
		}
		bool RecordElement(ofstream &flow_name, string element_name, int number){	//�������� � ���� ������� ���������; true - �������; false - ������ ������
			//cout << "RecordElement\n";
			return (flow_name << element_name << "=\"" << number << "\"\n").good();
		}
		bool RecordArray(ofstream &flow_name, string element_name, unsigned char *array, int array_size){	//�������� � ���� ������ � �������; true - �������; false - ������ ������
			//cout << "RecordArray\n";
			flow_name << element_name << "=(" << array_size;
			for(int i = 0; i < array_size; i++)
				flow_name << " " << ((int)array[i]);
			flow_name << ")\n";
			return flow_name.good();
		}
		bool RecordMoves(ofstream &flow_name){	//�������� � ���� ����; true - �������; false - ������ ������
			if (!RecordElement(flow_name, "m", moves.size()))
				return false;
			for(int i = 0; i < moves.size(); i++){
				if (!RecordNumber(flow_name, moves[i].size()))
					return false;
				if (moves[i].size())
					flow_name << " (";
				for(int n = 0; n < moves[i].size(); n++){
					RecordNumber(flow_name, moves[i][n].coordinate_index);
					flow_name << ' ';
					RecordNumber(flow_name, moves[i][n].prevState);
					flow_name << ' ';
					RecordNumber(flow_name, moves[i][n].newState);
					if (n < moves[i].size() - 1)
						flow_name << ", ";
				}
				if (moves[i].size())
					flow_name << ')';
				flow_name << '\n';
				if (!flow_name.good())
					return false;
			}
			return flow_name.good();
		}
		
		/*
		int PreviousSaveToFile(ofstream &record_file){	//���������� � ����; 0 - �������; 1 - ������ �������� �����; 2X - ������ ������ (X - ��� ������)
			if (record_file.is_open())
			{
				if (RecordItem(record_file, cols, " ") || RecordItem(record_file, rows, "\n"))
					return RecordFileError(record_file, 20);
				for(int i = 0; i < rows; i++)
					if (RecordLine(record_file, &(field[cols * i]), cols))
						return RecordFileError(record_file, 21);
				if (RecordItem(record_file, -1, "f") || RecordLine(record_file, field_value_mean, 5))
					return RecordFileError(record_file, 22);
				if (RecordItem(record_file, -1, "s") || RecordLine(record_file, ships, ships[0] + 1))
					return RecordFileError(record_file, 23);
				if (RecordItem(record_file, -1, "s") || RecordLine(record_file, ships_remain, ships_remain[0] + 1))
					return RecordFileError(record_file, 24);
				if (RecordItem(record_file, moves.size(), "\n"))
					return RecordFileError(record_file, 25);
				if (RecordMoves(record_file))
					return RecordFileError(record_file, 26);
			}
			else
			{
				cout << "�� ������� ������� ���� ��� ������\n";
				return 1;
			}
			record_file.close();
			return 0;
		}
		int RecordFileError(ofstream &flow_name, int error_type){	//��������� ����� � ���������� ����� ������
			flow_name.close();
			printf("RecordFileError(code = %d)!\n", error_type);
			return error_type;
		}
		int RecordItem(ofstream &flow_name, int value, string set_after_item){	//0 - �������; 1 - ������ ������
			if (value == -1)
				if (!(flow_name << set_after_item))
					return 1;
				else
					return 0;
			if (!(flow_name << value) || !(flow_name << set_after_item))
				return 1;
			return 0;
		}
		int RecordLine(ofstream &flow_name, unsigned char *array, int count){	//0 - �������; 1 - ������ ������
			for(int i = 0; i < count - 1; i++)
				if (RecordItem(flow_name, array[i], " "))
					return 1;
			if (RecordItem(flow_name, array[count - 1], "\n"))
				return 1;
			return 0;
		}
		int RecordMove(ofstream &flow_name, int moves_index, int move_index){	//0 - �������; 1 - ������ ������
			if (moves[moves_index].size())
			{
				if (RecordItem(flow_name, moves[moves_index][move_index].coordinate_index, " "))
					return 1;
				if (RecordItem(flow_name, moves[moves_index][move_index].prevState, " "))
					return 1;
				if (RecordItem(flow_name, moves[moves_index][move_index].newState, ""))
					return 1;
			}
			return 0;
		}
		int RecordMoves(ofstream &flow_name){									//0 - �������; 1 - ������ ������
			for(int i = 0, moves_size = moves.size(); i < moves_size; i++)
			{
				if (RecordItem(flow_name, -1, "m"))
					return 1;
				if (RecordItem(flow_name, moves[i].size(), ""))
					return 1;
				for(int n = 0; n < moves[i].size(); n++)
				{
					if (RecordItem(flow_name, -1, " "))
						return 1;
					if (RecordMove(flow_name, i, n))
						return 1;
				}
				if (RecordItem(flow_name, -1, "\n"))
					return 1;
			}
			return 0;
		}
		*/
		
		//������ � ���������
	private:
		void SetInSide(unsigned char set_what, int side, int len, int x, int y){	//������ set_what, len ������ � ����������� side
			int x_2 = XYChangesBySide(0, side), y_2 = XYChangesBySide(1, side), index;
			for(side = 0, x = x - x_2, y = y - y_2; side < len; x = x + x_2, y = y + y_2, side++)
			{
				index = cols * (y + y_2) + (x + x_2);
				AddCellValueChange(index, field[index], set_what);
				field[index] = set_what;
			}	
		}
		int CheckOneBlock(int x, int y){	//0 - �� � ������� / ���������� �� ��������� ����; >0 - ���-�� ��������
			int x_2, y_2, ships = 0;
			for(x_2 = -1; x_2 < 2; x_2++)
				if(x_2 + x >= 0 && x_2 + x < cols)
					for(y_2 = -1; y_2 < 2; y_2++)
						if(y_2 + y >= 0 && y_2 + y < rows)
							if(field[cols * (y_2 + y) + (x_2 + x)] == field_value_mean[4] || field[cols * (y_2 + y) + (x_2 + x)] == field_value_mean[3]
									|| field[cols * (y_2 + y) + (x_2 + x)] == field_value_mean[2])
								ships++;
			return ships;
		}
		int CheckSide(int side, int len, int x, int y){	//0 - �� � �������; 1 - �� ��������� ���� / �������
		    int x_2 = XYChangesBySide(0, side), y_2 = XYChangesBySide(1, side);
			for(side = 0, len--, x += x_2, y += y_2; side < len; x += x_2, y += y_2, side++)
				if ((x < 0 || x >= cols) || (y < 0 || y >= rows) || CheckOneBlock(x, y))
					return 1;
			return 0;
		}
		int FindShipEdge(int *x, int *y){	//���������� ����������� ������� � � *x � *y ������� ���������� ������� ������ �������
			int index = cols * *y + *x;
			if ((*x + 1 < cols && (field[index + 1] == field_value_mean[4] || field[index + 1] == field_value_mean[3] || field[index + 1] == field_value_mean[2])) ||
        		  (*x - 1 >= 0 && (field[index - 1] == field_value_mean[4] || field[index - 1] == field_value_mean[3] || field[index - 1] == field_value_mean[2])))
			{
				*x += (CalculateWhatInSide(*x, *y, 2, -1) - 1) * XYChangesBySide(0, 2);	//�������������� ������������ �������
				return 4;
			}
			*y += (CalculateWhatInSide(*x, *y, 1, -1) - 1) * XYChangesBySide(1, 1);
			return 3;
		}
		int CalculateWhatInSide(int x, int y, int side, int what){	//������� ���������� what � ����������� side �� EMPTY ��� SHOT ������
			int	x2 = XYChangesBySide(0, side), y2 = XYChangesBySide(1, side), pos;
			for(side = 0, pos = (cols * y) + x; !(field[pos] == field_value_mean[0] || field[pos] == field_value_mean[1]) && (pos >= 0 && pos < cols * rows); pos += (y2 * cols) + x2)
				if (what == -1)
				{
					if (field[pos] == field_value_mean[4] || field[pos] == field_value_mean[3] || field[pos] == field_value_mean[2])
						side++;
				}
				else
					if (field[pos] == what)
						side++;
			return side;
		}
		int SetShotAroundShips(int side, int len, int x, int y){	//���������� �������� ���-�� ���. SHOT
			int i = 0;
			i += ShotToWallOfSide(side, x, y, 1);
			i += ShotToMiddleOfSide(side, x, y, len);
			x += (len - 1) * XYChangesBySide(0, side);
			y += (len - 1) * XYChangesBySide(1, side);
			i += ShotToWallOfSide(side, x, y, 0);
			return i;
		}
		int ShotToMiddleOfSide(int side, int x, int y, int len){	//���������� ���������� ������������� SHOT
			int i, index, n = len, x2 = XYChangesBySide(0, side), y2 = XYChangesBySide(1, side);
			for(len = 0, x -= x2, y -= y2; n > 0; n--, x += x2, y += y2)
			{
				if (x2)
				{
					for(i = -1; i < 2; i += 2)
						if (y + i >= 0 && y + i < rows)
							if (field[cols * (y + i) + (x + x2)] == field_value_mean[0])
							{
								index = (cols * (y + i) + (x + x2));
								AddCellValueChange(index, field[index], field_value_mean[1]);
								field[index] = field_value_mean[1];
								len++;
							}
				}
				else
					for(i = -1; i < 2; i += 2)
						if (x + i >= 0 && x + i < cols)
							if (field[cols * (y + y2) + (x + i)] == field_value_mean[0])
							{
								index = (cols * (y + y2) + (x + i));
								AddCellValueChange(index, field[index], field_value_mean[1]);
								field[index] = field_value_mean[1];
								len++;
							}
			}
			return len;
		}
		int ShotToWallOfSide(int side, int x, int y, int back){	//���������� ���������� ������������� SHOT
			if (back)	//����� ����������� �� side ������� ��� ��������� SHOT
				if (side > 2)
					side -= 2;
				else
					side += 2;
			int count = 0, i, index, n, x2 = XYChangesBySide(0, side), y2 = XYChangesBySide(1, side);
			if (x2)
			{
				if (x + x2 >= 0 && x + x2 < cols)
					for(i = -1; i < 2; i++)
						if (y + i >= 0 && y + i < rows)
							if (field[cols * (y + i) + (x + x2)] == field_value_mean[0])
							{
								index = cols * (y + i) + (x + x2);
								AddCellValueChange(index, field[index], field_value_mean[1]);
								field[index] = field_value_mean[1];
								count++;
							}
			}
			else
				if (y + y2 >= 0 && y + y2 < rows)
					for(i = -1; i < 2; i++)
						if (x + i >= 0 && x + i < cols)
							if (field[cols * (y + y2) + (x + i)] == field_value_mean[0])
							{
								index = cols * (y + y2) + (x + i);
								AddCellValueChange(index, field[index], field_value_mean[1]);
								field[index] = field_value_mean[1];
								count++;
							}
			return count;
		}
	public:
		int ShotTo(int x, int y){	//����������; 0 - ��������� �� SHIP; 1 - ������ ���������� �������; 2 - ������� � ������ ������; 3 - ������������� �������, ��������� � SHOT, STRIKE, KILL; 4 - ������ ����������; 5 - ������ �������� ������
			if ((x < 0 || x >= cols) || (y < 0 || y >= rows))
				return 4;
			int index = (cols * y + x), side = -1, len = -1, x_ = x, y_ = y;
			unsigned char cell_value = field[index];
			if (field_value_mean[0] == cell_value)	//EMPTY
			{
				MakeMove();
				field[index] = field_value_mean[1];
				AddCellValueChange(index, field_value_mean[0], field_value_mean[1]);
				return 2;
			}
			else if (field_value_mean[4] == cell_value)	//SHIP
			{
				MakeMove();
				field[index] = field_value_mean[2];
				//AddCellValueChange(index, field_value_mean[4], field_value_mean[2]);
				side = FindShipEdge(&x_, &y_);	//����� ������� x_ � y_ ���������� ����������� ������� (������ / �������) ������ �������
				len = CalculateWhatInSide(x_, y_, side, -1);
				if (CalculateWhatInSide(x_, y_, side, field_value_mean[2]) == len)	//True - ������, False - ���������
				{
					//AddCellValueChange(index, field_value_mean[4], field_value_mean[3]);
					SetInSide(field_value_mean[3], side, len, x_, y_);
					TransformByIndexValueChange(index, field_value_mean[4], field_value_mean[3]);
					SetShotAroundShips(side, len, x_, y_);
					ships_remain[len] -= 1;
					if (!GetCountOfShipsRemain())
						return 1;
				}
				else
					AddCellValueChange(index, field_value_mean[4], field_value_mean[2]);
				return 0;
			}
			else if ((cell_value == field_value_mean[1]) ||
					 (cell_value == field_value_mean[2]) ||
					 (cell_value == field_value_mean[3]))	//SHOT STRIKE KILL
				{
					MakeMove();
					return 3;
				}
			else
				return 5;
		}
		
		int SetShip(int ship_len, int side, int x, int y){ //���������� �������; 0 - �������; 1 - ������ ���������; 2 - ������ ��������� ������ ������; 3 - ������ ��������� � �����������
			if ((x < 0 || x >= cols) || (y < 0 || y >= rows) || (side < 1 || side > 4))
				return 1;
			if (CheckOneBlock(x, y))
				return 2;
			if (ship_len == 1)
			{
				MakeMove();
				AddCellValueChange(cols * y + x, field_value_mean[0], field_value_mean[4]);
				field[cols * y + x] = field_value_mean[4];
				return 0;
			}
			if (!((AvailableSides(x, y, ship_len) / IntPow(10, 4 - side))%10))	//����������� ����������� ��������� � ������� side
				return 3;
			MakeMove();
			SetInSide(field_value_mean[4], side, ship_len, x, y);
			return 0;
		}
		
		int AvailableSides(int x, int y, int ship_len){ //���������� ��������� ������� �������������� ������ (���� - 1, ����� - 10, ��� - 100, ���� - 1000)
		    int side, sum = 0;
		    if (((x < 0 || x >= cols) || (y < 0 || y >= rows)) || CheckOneBlock(x, y))
		    	return 0;
		    for(side = 1; side < 5; side++)
				if (!CheckSide(side, ship_len, x, y))
					sum = sum + IntPow(10, 4 - side);
			return sum;
		}
	
		
		//���������� ��� �������
	private:
		int IsInCharRange(int number){	//0 - � ���������; 1 - �� ����������
			return (number < 0 || number > 255) ? 1 : 0;
		}
		int IsNewSizeAvailable(int cols_, int rows_){	//0 - ��������; 1 - ����������
			if ((cols_ < 1 || cols_ > 100000000) || (rows_ < 1 || rows_ > 100000000) || (cols_ > 1000000000 / rows_))
				return 1;
			return 0;
		}
		int MemoryAllocationError(int error_type){	//�������� error_type: 0 - ���������� ������ ������; 1 - �� ���������� ������ ������
			switch(error_type){
				case 0:
					printf("exit 1;\n");
					exit(1);
				case 1:
					return 1;
					break;
			}
		}
		int XYChangesBySide(int x_is_0_y_is_1, int side){	//�������� ��������� x, y � ����������� �� �������
		    if (x_is_0_y_is_1)	//� ����������� �� ����������� x = 0, 1, 0, -1;    y = 1, 0, -1, 0
		    	return (side < 3) ? 0 - side%2 : (side == 3) ? 1 : 0;
			return (side%2) ? 0 : (side == 2) ? 1 : -1;
		}
		int IntPow(int x, int y){	//���������� x � ������� y
			int a = 1;
			for(y; y > 0; y--)
					a *= x;
			return a;
		}
		int IsOneOfFieldValueMean(unsigned char number){	//0 - �������� �����-���� ��������� �� field_value_mean; 1 - �� ��������
			if (number == field_value_mean[0] || number == field_value_mean[1] || number == field_value_mean[2] || number == field_value_mean[3] || number == field_value_mean[4])
				return 0;
			return 1;
		}
		
		
		//��� ���������
	public:				//�������� ���������� ������!!!!!!!!!!!!!!!!!!!
		void PrintLastChange(){
			if (moves[moves.size() - 1].size() > 0)
				printf("index = %d, prev = %d, new = %d\n", moves[moves.size() - 1][0].coordinate_index, moves[moves.size() - 1][0].prevState, moves[moves.size() - 1][0].newState);
		}
		void PrintChanges(){	//���������� ��� ���� � ���������, ������������ � ���
			for(int n = 0, moves_count = (moves.size() - 1); n <= moves_count; n++)
			{
				printf("    moves[%d]\n", n);
				for (int i = 0, size = (moves[n].size() - 1); i <= size; i++){
	            	printf("index = %d, prev = %d, new = %d\n", moves[n][i].coordinate_index, moves[n][i].prevState, moves[n][i].newState);
	            	
	        	}
	        	if (n % 100 == 0)
	            	system("pause");
			}
		}
		void DebugField(){	//   ������� 
			printf("Object: %p\n", this);
			printf("cols: %p, %d\nrows: %p, %d\n", &cols, cols, &rows, rows);
			printf("field: %p\nfield_value_mean: %p\n", field, field_value_mean);
			for(int i = 0; i < 5; i++)
				cout << ((int)field_value_mean[i]) << " ";
			cout << endl;
			printf("ships: %p\nships_remain: %p\n", ships, ships_remain);
			for(int i = 0; i < ships[0]; i++)
				cout << ((int)ships[i]) << " ";
			cout << endl;
			printf("moves: %p\n", &moves);
		}









			/*	
			//�������� ����� ������ ��������:
			std::ios::in        // ��� ������
			std::ios::out       // ��� ������  
			std::ios::binary    // �������� �����
			std::ios::trunc     // ����� ���� �� ���� (����������)
			std::ios::app       // ���������� � ����� (append)
			std::ios::ate       // ������� � ������� � ����� �����
			
			//����������:
			std::ios::out | std::ios::trunc    // ���������� (�� ��������� ��� ofstream)
			std::ios::out | std::ios::app      // ���������� � �����
			std::ios::in | std::ios::out       // ������ � ������
			*/


		public:
    // ����� ��� ���������� � �������� ����
    bool saveToFile(const std::string& filename, bool append = false){	//��������� � ����; true - �������; false - ������ ������
        std::ofstream file(filename, std::ios::binary | (append ? ios::app : ios::trunc));
        if (!file.is_open())
			return false;
		printf("����� ����������\n");
		
		//���������� ������ ���� ������ ������ (� ���� �� ������ ������������)
		size_t record_size = SizeForSave();
    	file.write(reinterpret_cast<const char*>(&record_size), sizeof(record_size));
    	
        //���������� ������� ����
        file.write(reinterpret_cast<const char*>(&cols), sizeof(cols));
        file.write(reinterpret_cast<const char*>(&rows), sizeof(rows));

        //���������� ������� field, field_value_mean, ships, ships_remain
        file.write(reinterpret_cast<const char*>(field), cols * rows);
        file.write(reinterpret_cast<const char*>(field_value_mean), 5);
        
        //��� ships � ships_remain: ������� �����, ����� ������
        unsigned char ships_length = ships[0] + 1; // +1 ������ ��� ships[0] ������ ������������ �����
        file.write(reinterpret_cast<const char*>(&ships_length), sizeof(ships_length));
        file.write(reinterpret_cast<const char*>(ships), ships_length);
        
        file.write(reinterpret_cast<const char*>(&ships_length), sizeof(ships_length));
        file.write(reinterpret_cast<const char*>(ships_remain), ships_length);

        //���������� moves
        size_t moves_count = moves.size();
        file.write(reinterpret_cast<const char*>(&moves_count), sizeof(moves_count));
        
        for (const auto &move : moves){
            size_t changes_count = move.size();
            file.write(reinterpret_cast<const char*>(&changes_count), sizeof(changes_count));
            
            for (const auto &change : move){
                file.write(reinterpret_cast<const char*>(&change.coordinate_index), sizeof(change.coordinate_index));
                file.write(reinterpret_cast<const char*>(&change.prevState), sizeof(change.prevState));
                file.write(reinterpret_cast<const char*>(&change.newState), sizeof(change.newState));
            }
        }
		printf("��������? %d\n", file.good());
        return file.good();
    }
    size_t SizeForSave(){	//���������� ������ ������ � ������ ��� �������� � �����
		size_t size = sizeof(size_t);
    
	    size += sizeof(cols) + sizeof(rows);			//������� ����
	    size += cols * rows;							//field
	    size += 5;										//field_value_mean
	    size += sizeof(unsigned char) * (ships[0] + 1);	//ships
	    size += sizeof(unsigned char) * (ships[0] + 1);	//ships_remain
	    
	    size += sizeof(size_t);							//moves.size()
	    for (const auto &move : moves){					//moves[n].size()
	    	/**
			for (size_t i = 0; i < moves.size(); i++){
                file.read(reinterpret_cast<char*>(&moves[i][j].coordinate_index), sizeof(moves[i][j].coordinate_index));
                file.read(reinterpret_cast<char*>(&moves[i][j].prevState), sizeof(moves[i][j].prevState));
                file.read(reinterpret_cast<char*>(&moves[i][j].newState), sizeof(moves[i][j].newState));
            }
            */
	        size += (sizeof(size_t) + move.size() * (sizeof(int) + sizeof(unsigned char) + sizeof(unsigned char)));	//coordinate_index, prevState, newState
	    }
	    
	    return size;
	}

    // ����� ��� �������� �� ��������� �����
    bool loadFromFile(const std::string& filename, bool second_field = false){	//�������� �� �����; true - �������; false - ������ ������
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
			return false;
		
		cout << "�����\n";
		
		size_t record_size;
		
		file.read(reinterpret_cast<char*>(&record_size), sizeof(record_size));	//������ ������ ������� � �����
		if (second_field){
			file.seekg(record_size, std::ios::beg);	//���������� ������ ������ �� � ������� �� �����
	        file.read(reinterpret_cast<char*>(&record_size), sizeof(record_size));	//������ ������ ������, �� �� ���������� ��� ��� ��������
	    }
	    
	    
	    
	    
	    
	    
        //����������� ������������ �������
        cleanup();

        //������ ������� ����
        file.read(reinterpret_cast<char*>(&cols), sizeof(cols));
        file.read(reinterpret_cast<char*>(&rows), sizeof(rows));

        //�������� ������ ��� ��������
        size_t field_size = cols * rows;
        field = new unsigned char[field_size];
        field_value_mean = new unsigned char[5];

        //������ ������� field � field_value_mean
        file.read(reinterpret_cast<char*>(field), field_size);
        file.read(reinterpret_cast<char*>(field_value_mean), 5);
        
        //printf("F0 � 99: %d, %d\n", field[0], field[99]);
        //printf("FVM: %d, %d, %d, %d, %d\n", field_value_mean[0], field_value_mean[1], field_value_mean[2], field_value_mean[3], field_value_mean[4]);

        //������ ships
        unsigned char ships_length;
        file.read(reinterpret_cast<char*>(&ships_length), sizeof(ships_length));
        ships = new unsigned char[ships_length];
        file.read(reinterpret_cast<char*>(ships), ships_length);

        //������ ships_remain
        file.read(reinterpret_cast<char*>(&ships_length), sizeof(ships_length));
        ships_remain = new unsigned char[ships_length];
        file.read(reinterpret_cast<char*>(ships_remain), ships_length);
        
        
        
        //printf("ships ������������ �����: %d. %d, %d, %d, %d\n", ships[0], ships[1], ships[2], ships[3], ships[4], ships[5]);
        //printf("ships_remain ������������ �����: %d. %d, %d, %d, %d\n", ships_remain[0], ships_remain[1], ships_remain[2], ships_remain[3], ships_remain[4], ships_remain[5]);

        //������ moves
        size_t moves_count;
        file.read(reinterpret_cast<char*>(&moves_count), sizeof(moves_count));
        moves.resize(moves_count);

		printf("moves.size() = %d\n", moves_count);
        for (size_t i = 0; i < moves_count; i++){
            size_t changes_count;
            file.read(reinterpret_cast<char*>(&changes_count), sizeof(changes_count));
            moves[i].resize(changes_count);

            for (size_t j = 0; j < changes_count; j++){
                file.read(reinterpret_cast<char*>(&moves[i][j].coordinate_index), sizeof(moves[i][j].coordinate_index));
                file.read(reinterpret_cast<char*>(&moves[i][j].prevState), sizeof(moves[i][j].prevState));
                file.read(reinterpret_cast<char*>(&moves[i][j].newState), sizeof(moves[i][j].newState));
            }
        }

        return file.good();
    }

private:
    void cleanup() {
        delete[] field;
        delete[] field_value_mean;
        delete[] ships;
        delete[] ships_remain;
        moves.clear();
    }









};




class SeaBattleGame : public SeaBattleField //��������� � ���������� ����
{	
	private:
		char *FieldSymbol;	//������� ����������� ������ �� ����� ����
	public:
		SeaBattleGame(int cols_ = 10, int rows_ = 10) : SeaBattleField(cols_, rows_) {
			FieldSymbol = 0;
			FieldSymbol = new char[5];
			if (!FieldSymbol)
				exit(1);
			FieldSymbol[0] = '-';	//������ ������
			FieldSymbol[1] = '*';	//��������� ������
			FieldSymbol[2] = 'X';	//������� ������ �������
			FieldSymbol[3] = '#';	//������ ����������� �������
			FieldSymbol[4] = 'S';	//����� ������ �������
		}
		~SeaBattleGame() {
			if (FieldSymbol)
				delete[] FieldSymbol;
			FieldSymbol = 0;
		}
		SeaBattleGame(const SeaBattleGame& other){
			FieldSymbol = 0;
			FieldSymbol = new char[5];
			if (!FieldSymbol)
				exit(1);
			FieldSymbol[0] = other.FieldSymbol[0];	//������ ������
			FieldSymbol[1] = other.FieldSymbol[1];	//��������� ������
			FieldSymbol[2] = other.FieldSymbol[2];	//������� ������ �������
			FieldSymbol[3] = other.FieldSymbol[3];	//������ ����������� �������
			FieldSymbol[4] = other.FieldSymbol[4];	//����� ������ �������
		}
		void PrintColored(const string& text, int color) {
	        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	        SetConsoleTextAttribute(hConsole, color + 1);
	        cout << text;
	        SetConsoleTextAttribute(hConsole, 7);
    	}
		void DrawSymbol(int index){	//��������� �������� index ����, � ���� �������� �������
			unsigned char value = GetValueOfCellByIndex(index);
			string myString(1, FieldSymbol[0]);
			if (value == GetValueOfValueMean(0))
				PrintColored(myString, 5);
				//cout << FieldSymbol[0];
			else if (value == GetValueOfValueMean(1))
				cout << FieldSymbol[1];
			else if (value == GetValueOfValueMean(2))
				cout << FieldSymbol[2];
			else if (value == GetValueOfValueMean(3))
				cout << FieldSymbol[3];
			else if (value == GetValueOfValueMean(4))
				cout << FieldSymbol[4];
		}
		void DrawFields(SeaBattleGame &enemy_field) 
		{
	        int y;
	        
	        PrintWordNTimes(" ", GetCols() - 1);
	        cout << "���� ���� ";
	        PrintWordNTimes(" ", 2 * GetCols() + 3);
	        cout << "���� �����" << endl;
	        
	        draw_top_letters();
	        cout << "          ";
	        enemy_field.draw_top_letters();
	        
	        for(cout << endl, y = 0; y < GetRows(); y++, cout << " |\n") {
	            draw_row(y, 1);
	            printf(" |        ");
	            enemy_field.draw_row(y, 0);
	        }
	        cout << endl;
	    }
		
	    void draw_row(int y, int owner) //������ � ������ �������� �����
		{ 
	        int x, value;
	        for(cout << setfill(' ') << setw(2) << y << "|", x = 0; x < GetCols(); x++) {
	            value = GetValueOfCell(x, y);
	            if (value == GetValueOfValueMean(0)) 
				{
	                cout << " " << FieldSymbol[0];
	            } 
				else if (value == GetValueOfValueMean(4)) 
				{
	                if (owner)
	                    cout << " " << FieldSymbol[4];
	                else
	                    cout << " " << FieldSymbol[0];
	            } 
				else 
				{
	                for (int i = 0; i < 5; i++) {
	                    if (value == GetValueOfValueMean(i)) {
	                        cout << " " << FieldSymbol[i];
	                        break;
	                    }
	                }
	            }
	        }
	    }
		
		void DrawField(){	//���������� 1 ���� �������� ���������
			for(int index = 0; index < GetCols() * GetRows(); index++){
				DrawSymbol(index);
				if (((index + 1) % GetCols() == 0) && (index > 0))
					cout << endl;
				else
					cout << " ";
			}
			cout << endl;
		}
		
		void draw_top_letters()
		{						//��������� ������ �������������� ���������
			short i;
			for(printf("   "), i = 0; i < this->GetCols(); i++)
				printf(" %c", i + 'A');
		}
		
		void PrintWordNTimes(string word, int n){ 				//������� ������ n ���
			for(n; n > 0; n--)
				cout << word;
		}
		
		void PrintFieldValues(){	//���������� 1 ���� ��������� ����������
			for(int index = 0; index < GetCols() * GetRows(); index++){
				cout << GetValueOfCellByIndex(index);
				if (((index + 1) % GetCols() == 0) && (index > 0))
					cout << endl;
				else
					cout << " ";
			}
			cout << endl;
		}
		void PrintFieldIndexs(){	//���������� ������� 1 ����
			for(int index = 0; index < GetCols() * GetRows(); index++){
				cout << index;
				if (((index + 1) % GetCols() == 0) && (index > 0))
					cout << endl;
				else
					cout << " ";
			}
		}
		void PrintFieldValueMean(){	//���������� ��������, �������� ������������ EMPTY, SHOT, ..., SHIP
			cout << "�����������:\nEMPTY - " << GetValueOfValueMean(0) << "\nSHOT - " << GetValueOfValueMean(1)
			<< "\nSTRIKE - " << GetValueOfValueMean(2) << "\nKILL - " << GetValueOfValueMean(3)
			<< "\nSHIP - " << GetValueOfValueMean(4) << "\n";
		}
		
		void SetShipsRandomly() 
		{
	        ClearField();
	        ResetShipsRemain();
	        srand(clock()); // ������������� ����������
	        
	        int max_len = GetMaxShipLen();
	        for (int len = max_len; len >= 1; len--) 
			{
	            int count = GetShipsRemainCountOfNLen(len);
	            for (int i = 0; i < count; i++) 
				{
	                if (!PlaceRandomShip(len)) {
	                    i--;
	                }
	            }
	        }
    	}
    
	    bool PlaceRandomShip(int len) {
	        int attempts = 0;
	        while (attempts < 1000) {
	            int x = rand() % GetCols(); // ��������� ���������� X
	            int y = rand() % GetRows(); // ��������� ���������� Y
	            
	            if (len == 1) {
	            	// ��� 1-��������� �������
	                if (SetShip(1, 1, x, y) == 0) {
	                    return true;
	                }
	            } else {
	            	// ��� ������������� ��������
	                int available_sides = AvailableSides(x, y, len);
	                if (available_sides > 0) {
	                    for (int side = 1; side <= 4; side++) {
	                        if ((available_sides / IntPow(10, 4-side)) % 10) {
	                            if (SetShip(len, side, x, y) == 0) {
	                                return true;
	                            }
	                        }
	                    }
	                }
	            }
	            attempts++;
	        }
	        return false;
	    }
	    
	private:
	    int IntPow(int x, int y) 
		{
	        int result = 1;
	        for (int i = 0; i < y; i++) {
	            result *= x;
	        }
	        return result;
	    }
	
	
	//����� �������
	public:
		void SetShipsTest(){
			SetShip(4, 2, 0, 0);
			SetShip(3, 2, 0, 2);
			SetShip(3, 3, 5, 0);
			SetShip(2, 2, 0, 4);
			SetShip(2, 2, GetCols() - 4, 5);
			SetShip(2, 2, 5, GetRows() - 1);
			SetShip(1, 2, 0, 6);
			SetShip(1, 2, GetCols() - 1, GetRows() - 1);
			SetShip(1, 2, GetCols() - 1, 0);
			SetShip(1, 2, GetCols() - 1, 4);
		}
		void FullGameTest(){	//�������� ���� �� ����� ����
			SetShip(4, 2, 0, 0);
			SetShip(3, 2, 0, 2);
			SetShip(3, 3, 5, 0);
			SetShip(2, 2, 0, 4);
			SetShip(2, 2, GetCols() - 4, 5);
			SetShip(2, 2, 5, GetRows() - 1);
			SetShip(1, 2, 0, 6);
			SetShip(1, 2, GetCols() - 1, GetRows() - 1);
			SetShip(1, 2, GetCols() - 1, 0);
			SetShip(1, 2, GetCols() - 1, 4);
			
			ShotTo(0, 0);
			ShotTo(1, 0);
			ShotTo(2, 0);
			ShotTo(3, 0);
			
			ShotTo(0, 2);
			ShotTo(1, 2);
			ShotTo(2, 2);
			
			ShotTo(0, 4);
			ShotTo(1, 4);
			
			ShotTo(0, 6);
			
			ShotTo(5, 0);
			ShotTo(5, 2);
			ShotTo(5, 1);
			
			ShotTo(-1, 2);
			ShotTo(1, -2);
			ShotTo(1, 3);
			ShotTo(4, 2);
			ShotTo(6, 6);
			ShotTo(1, 7);
			ShotTo(7, 5);
			ShotTo(1, 9);
			ShotTo(10, 2);
			ShotTo(1, 10);
			
			ShotTo(GetCols() - 1, 0);
			
			ShotTo(GetCols() - 1, GetRows() - 1);
			
			ShotTo(GetCols() - 1, 4);
			
			ShotTo(6, 5);
			ShotTo(7, 5);
			
			ShotTo(5, GetRows() - 1);
			ShotTo(6, GetRows() - 1);
		}
};


//���� ������� ������:
//SeaBattleField:
//	
//SeaBattleGame:
//	DrawColored();	//���������� ������ ������
//	DrawFields();	//����� �������������� ��� ���� �� �����������. ��� ������ � ����� �� ������� �� ��� ��������� ���������� (������ �����, ����� �����).
//	Menu(); //������ ����� ����, ��������� ��������� ����, �������� ��������� ���� (��������� � Settings()), ��������� ���������
//	Settings(); //�������� ������ �������� ������ �������� ���� � ���������� ��������, ������� ����� � ���� (����� �������������� ��������� �������� ��������������, �.�. ������ ����� �������. ����� ��������� ���� �� ������ ��������� ��������� �������� � ����������� (�������� ����, ������� ��������� ����, ships_remain ���������� � ships)
//	GetReadyForGame(); //������� ��� ��������� �������� ��� ���� (������� ����, ����� ����� � ������������� �������� � ����).
//	Game(SeaBattleField &field_2); ���� static Game(SeaBattleField &field_1, SeaBattleField &field_2);	//��� ���������� ����. � ��� ����������� ������ ����, ���������� ��� � �.�.
//	Seed(); //���������� "���������" �����, ������������ ��� ��������� ����-���� � ���������
//	SetShipsRandomly(); //���������� ������� "���������" �������
//	
//SeaBattleBot() //��� ����� ����� �� ���� ������� ����
//	//������ ���� ���������� ����
//	
//	BotMakeMove(); //������� ���
//	ShotLogic(); //����������, ���� ��������
	

class SeaBattleGameMenu {
private:
    SeaBattleGame* player1;
    SeaBattleGame* player2;
    int ships_count_main;
    int player1_color;
    int player2_color;
    int ship_len_limit;
    
    enum MENU {
        NEW_GAME = 0,
        CHANGE_SHIPS,
        RESET_SHIPS_COUNT,
        CHANGE_MAX_SHIP_LEN,
        INSTRUCTION,
        EXIT,
        MENU_LEN
    };
    
    enum GAME_MODE {
        PLAYER_VS_PLAYER = 1,
        PLAYER_VS_COMPUTER,
        BACK_TO_MENU = 2
    };

public:
    SeaBattleGameMenu() {
        player1 = nullptr;
        player2 = nullptr;
        ships_count_main = 1234;
        player1_color = 1;
        player2_color = 2;
        ship_len_limit = 5;
    }
    
    ~SeaBattleGameMenu() {
        if (player1) delete player1;
        if (player2) delete player2;
    }
    
    void Run() {
        cout << "����� ���������� � ���� '������� ���'.\n";
        MainMenu();
    }

private:
    void MainMenu() {
        int choice = -1;
        
        while (true) {
            cout << "�� ���������� � ������� ����. ������� ����� ���� ��������, ������� ������ ���������.\n";
            PrintMenu();
            
            if (!(cin >> choice) || choice < 0 || choice >= MENU_LEN) {
                InputErrorMessage();
                continue;
            }
            
            system("cls");
            
            switch (choice) {
                case NEW_GAME:
                    NewGame();
                    break;
                case CHANGE_SHIPS:
                    ChangeShipsCount();
                    break;
                case RESET_SHIPS_COUNT:
                    ResetShipsCount();
                    break;
                case CHANGE_MAX_SHIP_LEN:
                    ChangeMaxShipLen();
                    break;
                case INSTRUCTION:
                    PrintInstruction();
                    break;
                case EXIT:
                    if (Exit()) return;
                    break;
            }
        }
    }
    
    void PrintMenu() {
        cout << "\t" << NEW_GAME << " ������ ����� ����\n";
        cout << "\t" << CHANGE_SHIPS << " �������� ���������� ��������\n";
        cout << "\t" << RESET_SHIPS_COUNT << " �������� ���������� ��������\n";
        cout << "\t" << CHANGE_MAX_SHIP_LEN << " �������� ������������ ����� ������� � ����\n";
        cout << "\t" << INSTRUCTION << " ���������� �� ����\n";
        cout << "\t" << EXIT << " �����\n";
    }
    
    void NewGame() {
        int choice = -1;
        
        while (true) {
            cout << "�������� ����� ����:\n";
            cout << "\t" << PLAYER_VS_PLAYER << " ������� � ���������\n";
            cout << "\t" << BACK_TO_MENU << " ����� � ����\n";
            
            if (!(cin >> choice) || choice < PLAYER_VS_PLAYER || choice > BACK_TO_MENU) {
                InputErrorMessage();
                continue;
            }
            
            system("cls");
            
            switch (choice) {
                case PLAYER_VS_PLAYER:
                    PlayerVsPlayer();
                    return;
                case BACK_TO_MENU:
                    return;
            }
        }
    }
    
    void PlayerVsPlayer() {
        if (player1) delete player1;
        if (player2) delete player2;
        
        player1 = new SeaBattleGame();
        player2 = new SeaBattleGame();
        
        ConfigureShipsCount(*player1);
        ConfigureShipsCount(*player2);
        
        player1_color = InputColorID("������� ������");
        player2_color = InputColorID("������� ������");
        
        cout << "������ ������� ";
        PrintColored("������� ������ ", player1_color);
        cout << "����������� �������.\n";
        
        if (Confirm("���������� ������� �������?")) {
            SetShipsManually(*player1);
        } else {
            SetShipsRandomly(*player1);
        }
        
        system("cls");
        cout << "���� ������� ������� �����������.\n������ ������� ";
        PrintColored("������� ������ ", player2_color);
        cout << "����������� �������.\n";
        
        if (Confirm("���������� ������� �������?")) {
            SetShipsManually(*player2);
        } else {
            SetShipsRandomly(*player2);
        }
        
        StartGame();
    }
    
    void ConfigureShipsCount(SeaBattleGame& game) {
        game.ResetShips();
    }
    
    void SetShipsManually(SeaBattleGame& game) {
        cout << "����� ������ ����������� ��������\n";
        game.DrawField();
        
        int ship_len = game.GetMaxShipLen();
        while (ship_len > 0) {
            int count = game.GetShipsRemainCountOfNLen(ship_len);
            for (int i = 0; i < count; i++) {
                system("cls");
                cout << "����������� " << ship_len << "-��������� ������� (" << (i+1) << "/" << count << ")\n";
                game.DrawField();
                
                if (ship_len == 1) {
                    int x, y;
                    cout << "������� ���������� ��� 1-��������� �������: ";
                    while (!GetShotCoordinates(x, y) || !game.SetShip(1, 1, x, y)) {
                        InputErrorMessage();
                        cout << "������� ���������� ��� 1-��������� �������: ";
                    }
                } 
				else 
				{
                    int x, y, side;
                    cout << "������� ���������� ��������� ����� " << ship_len << "-��������� �������: ";
                    while (!GetShotCoordinates(x, y)) {
                        InputErrorMessage();
                        cout << "������� ���������� ��������� ����� " << ship_len << "-��������� �������: ";
                    }
                    
                    int available_sides = game.AvailableSides(x, y, ship_len);
                    if (available_sides == 0) {
                        cout << "�� ���� ����� ������ ���������� �������. ���������� ������ �����.\n";
                        i--;
                        continue;
                    }
                    
                    cout << "��������� �����������: ";
                    if ((available_sides / 1000) % 10) cout << "1(�����) ";
                    if ((available_sides / 100) % 10) cout << "2(������) ";
                    if ((available_sides / 10) % 10) cout << "3(����) ";
                    if (available_sides % 10) cout << "4(�����) ";
                    cout << "\n�������� �����������: ";
                    
                    while (!(cin >> side) || side < 1 || side > 4 || !((available_sides / IntPow(10, 4-side)) % 10)) {
                        InputErrorMessage();
                        cout << "�������� �����������: ";
                    }
                    
                    if (game.SetShip(ship_len, side, x, y) != 0) {
                        cout << "�� ������� ���������� �������. ���������� �����.\n";
                        i--;
                    }
                }
            }
            ship_len--;
        }
        
        system("cls");
        cout << "��� ������� ������� �����������!\n";
        game.DrawField();
        system("pause");
    }
    
    void SetShipsRandomly(SeaBattleGame& game) {
        game.SetShipsRandomly(); // ����� � ����� �� ���������, �� � ������ SeaBattleGame
        cout << "������� ����������� ��������� �������.\n";
        game.DrawField();
        
        if (!Confirm("����� ������������ ��������?")) {
            SetShipsRandomly(game);
        }
    }
    
    void StartGame() {
        system("cls");
        cout << "��� ������� ������� �����������.\n";
        cout << "���������� ������� ���. �������� ����!\n";
        system("pause");
        
        bool game_over = false;
        bool player1_turn = true;
        
        while (!game_over) 
		{
            if (!player1_turn) {
                cout << "\a����� ������� ����!\n";
                cout << "������ ����� ������ ";
                PrintColored("������ �����", player2_color);
                cout << ".\n";
                system("pause");
                system("cls");
            }
            
            bool turn_result;
	        if (player1_turn) {
	            turn_result = PlayerTurn(*player1, *player2, "������� ������", player1_color, player2_color, game_over, player1_turn);
	        } else {
	            turn_result = PlayerTurn(*player2, *player1, "������� ������", player2_color, player1_color, game_over, player1_turn);
	        }
	        
	        if (turn_result) {
	            game_over = true;
	        } 
			else if (!game_over) { // ������� ���� ������� ������
	            player1_turn = !player1_turn;
	        }
	        
	        if (!game_over) {
            	system("pause");
        	}
        }
        
        system("cls");
        cout << "\a��� ������� ";
        if (player1_turn) {
            PrintColored("������� ������ ", player2_color);
        } else {
            PrintColored("������� ������ ", player1_color);
        }
        cout << "���������!\n������ ������� ";
        if (player1_turn) {
            PrintColored("������ �����", player1_color);
        } else {
            PrintColored("������ �����", player2_color);
        }
        cout << ".\n������� �� ����!\n";
        system("pause");
        system("cls");
    }
    
    bool GetShotCoordinates(int& x, int& y) {
        char input_x;
        cout << "������� ���������� ��� �������� (��������, A5): ";
        
        if (!(cin >> input_x >> y)) {
            return false;
        }
        
        if (input_x >= 'A' && input_x <= 'Z') {
            x = input_x - 'A';
        } else if (input_x >= 'a' && input_x <= 'z') {
            x = input_x - 'a';
        } else {
            return false;
        }
        
        if (player1 && (x < 0 || x >= player1->GetCols() || y < 0 || y >= player1->GetRows())) {
            return false;
        }
        
        return true;
    }
    
    void ChangeShipsCount() {
        cout << "��������� ���������� ��������:\n";
        PrintShipsCount(ships_count_main);
        
        int ship_len, new_count;
        cout << "������� ����� ������� ��� ���������: ";
        while (!(cin >> ship_len) || ship_len < 1 || ship_len >= ship_len_limit) {
            InputErrorMessage();
            cout << "������� ����� ������� ��� ���������: ";
        }
        
        int current_count = (ships_count_main / IntPow(10, ship_len-1)) % 10;
        cout << "������� ���������� " << ship_len << "-�������� ��������: " << current_count << "\n";
        cout << "������� ����� ����������: ";
        
        while (!(cin >> new_count) || new_count < 0 || new_count > 9) {
            InputErrorMessage();
            cout << "������� ����� ����������: ";
        }
        
        ships_count_main = ships_count_main - (current_count * IntPow(10, ship_len-1)) + (new_count * IntPow(10, ship_len-1));
        
        cout << "���������� �������� ��������.\n";
        PrintShipsCount(ships_count_main);
        system("pause");
        system("cls");
    }
    
    void ResetShipsCount() {
        if (Confirm("�������� ���������� �������� � ����������� ���������?")) {
            ships_count_main = 1234;
            ship_len_limit = 5;
            cout << "�������� �������� � �����������.\n";
        } else {
            cout << "�������� �� ���� ��������.\n";
        }
        system("pause");
        system("cls");
    }
    
    void ChangeMaxShipLen() {
        int new_max_len;
        cout << "������� ������������ ����� �������: " << (ship_len_limit - 1) << "\n";
        cout << "������� ����� ������������ ����� (1-9): ";
        
        while (!(cin >> new_max_len) || new_max_len < 1 || new_max_len > 9) {
            InputErrorMessage();
            cout << "������� ����� ������������ ����� (1-9): ";
        }
        
        if (Confirm("��������� ���������?")) {
            ship_len_limit = new_max_len + 1;
            
            if (ship_len_limit < 5) {
                ships_count_main = ships_count_main % IntPow(10, ship_len_limit - 1);
            }
            
            cout << "������������ ����� �������� �� " << new_max_len << ".\n";
        } else {
            cout << "��������� ��������.\n";
        }
        
        system("pause");
        system("cls");
    }
    
    void PrintInstruction() {
        cout << "///////////////////////////////////����������///////////////////////////////////\n\n";
        cout << "���� '������� ���'\n\n";
        cout << "���� ����: ������ ���������� ��� ������� ����������.\n\n";
        cout << "�������:\n";
        cout << "- ������� ���� �������� 10x10 ������\n";
        cout << "- ������� ������������� ����������� ��� �������������\n";
        cout << "- ������� �� ����� ������������� ���� � ������\n";
        cout << "- ��� ��������� � ������� ������ �������������� ���\n";
        cout << "- ��� ����������� ������� ���������� ������ ������ ����\n\n";
        cout << "������� �� ����:\n";
        cout << "'-' - ������ ������\n";
        cout << "'*' - ������\n"; 
        cout << "'X' - ������� �������\n";
        cout << "'#' - ������������ �������\n";
        cout << "'S' - ��� ������� (����� ������ �� ����� ����)\n\n";
        cout << "����������:\n";
        cout << "- ��� �������� ������� ����� � ����� (��������: A5)\n";
        cout << "- ����� �� A �� J - �������������� ����������\n";
        cout << "- ����� �� 0 �� 9 - ������������ ����������\n";
        cout << "////////////////////////////////����� ����������////////////////////////////////\n\n";
        system("pause");
        system("cls");
    }
    
    bool Exit() {
        if (Confirm("�� ����� ������ �����?")) {
            return true;
        }
        system("cls");
        return false;
    }
    
    void InputErrorMessage() {
        system("cls");
        cout << "������������ ����! ���������� ��� ���.\n\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    
    bool Confirm(const string& message) {
        int choice;
        while (true) {
            cout << message << "\n'0' - ���� ��.\n'1' - ���� ���.\n";
            if (cin >> choice && (choice == 0 || choice == 1)) {
                return choice == 0;
            }
            InputErrorMessage();
        }
    }
    
    void PrintColored(const string& text, int color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color + 1);
        cout << text;
        SetConsoleTextAttribute(hConsole, 7);
    }
    
    int InputColorID(const string& player_name) {
        int color;
        while (true) {
            cout << "��������� ����� ��� " << player_name << ":\n";
            cout << "1 - �������\n2 - �������\n3 - ������\n4 - �����\n";
            cout << "5 - ����������\n6 - �������\n7 - �����\n";
            cout << "�������� ���� (1-7): ";
            
            if (cin >> color && color >= 1 && color <= 7) {
                cout << "������: ";
                PrintColored(player_name, color);
                cout << "\n";
                
                if (Confirm("���� ���� ��������?")) {
                    return color;
                }
            } else {
                InputErrorMessage();
            }
        }
    }
    
    void PrintShipsCount(int ships_count) {
        cout << "������� � ����:\n";
        for (int n = ship_len_limit - 1; n > 0; n--) {
            int count = (ships_count / IntPow(10, n-1)) % 10;
            if (count > 0) {
                cout << "  " << count << " x " << n << "-��������\n";
            }
        }
        cout << "\n";
    }
    
    int IntPow(int x, int y) {
        int result = 1;
        for (int i = 0; i < y; i++) {
            result *= x;
        }
        return result;
    }
    
    int CountShips(int ships_count) {
        int total = 0;
        for (int i = ship_len_limit - 1; i > 0; i--) {
            total += (ships_count / IntPow(10, i-1)) % 10;
        }
        return total;
    }
    
private:
    void InGameMenu(SeaBattleGame& current_player, SeaBattleGame& enemy, bool& game_over, bool& player1_turn) {
        int choice = -1;
        
        while (true) {
            system("cls");
            current_player.DrawFields(enemy);
            
            cout << "\n=== ������� ���� ===\n";
            cout << "1 - ���������� ����\n";
            cout << "2 - ��������� ����\n";
            cout << "3 - ��������� ������ ����\n";
            cout << "4 - ����� � ������� ����\n";
            cout << "5 - ��������� ���������\n";
            cout << "�������� ��������: ";
            
            if (!(cin >> choice) || choice < 1 || choice > 5) {
                InputErrorMessage();
                continue;
            }
            
            switch (choice) {
                case 1:
                    // ���������� ����
                    return;
                    
                case 2:
                    // ��������� ����
                    SaveCurrentGame(current_player, enemy, player1_turn);
                    break;
                    
                case 3:
                    // ��������� ������ ����
                    if (LoadDifferentGame(current_player, enemy, player1_turn)) {
                        game_over = false;
                        return;
                    }
                    break;
                    
                case 4:
                    // ����� � ������� ����
                    if (Confirm("�� ����� ������ ����� � ������� ����? ������� �������� ����� �������.")) {
                        game_over = true;
                        return;
                    }
                    break;
                    
                case 5:
                    // ��������� ���������
                    if (Confirm("�� ����� ������ ��������� ���������?")) {
                        exit(0);
                    }
                    break;
            }
        }
    }
    
    void SaveCurrentGame(SeaBattleGame& player1, SeaBattleGame& player2, bool current_turn) {
        string filename;
        cout << "������� ��� ����� ��� ���������� (��� ��������): ";
        cin >> filename;
        
        // ��������� ���������� ���� �����
        if (filename.length() < 4 || filename.substr(filename.length() - 4) != ".txt") 
		{
		    filename += ".txt";
		}
        
        // ��������� ���������� � ������� ����
        ofstream info_file("game_info.txt");
        if (info_file.is_open()) {
            info_file << current_turn << endl;
            info_file.close();
        }
        
        // ��������� ���� ���� (���������������)
        /*
        if (SeaBattleField::SaveGameSBF(player1, player2, filename.c_str()) == 0) // c_str - ���������� ��������� �� char (const char*)
		{
            cout << "���� ������� ��������� � ����: " << filename << endl;
        } else {
            cout << "������ ��� ���������� ����!" << endl;
        }
        */
        system("pause");
    }
    
    bool LoadDifferentGame(SeaBattleGame& player1, SeaBattleGame& player2, bool& current_turn) {
        string filename;
        cout << "������� ��� ����� ��� ��������: ";
        cin >> filename;
        
        // ��������� ���������� ���� �����
        if (filename.find(".txt") == string::npos) {
            filename += ".txt";
        }
        
        // ��������� ����
        if (SeaBattleField::LoadGameSBF(player1, player2, filename.c_str()) == 0) {
            // ��������� ���������� � ����
            ifstream info_file("game_info.txt");
            if (info_file.is_open()) {
                info_file >> current_turn;
                info_file.close();
            }
            
            cout << "���� ������� ��������� �� �����: " << filename << endl;
            system("pause");
            return true;
        } else {
            cout << "������ ��� �������� ����!" << endl;
            system("pause");
            return false;
        }
    }

	bool PlayerTurn(SeaBattleGame& current_player, SeaBattleGame& enemy, const string& player_name, 
	               int player_color, int enemy_color, bool& game_over, bool& player1_turn) {
	    bool extra_turn = true;
	    
	    while (extra_turn && !game_over) {
	        system("cls");
	        current_player.DrawFields(enemy);
	        
	        cout << "��� ";
	        PrintColored(player_name, player_color);
	        cout << "\n";
	        
	      
	        cout << "������� '0' ��� ������ ���� ��� ���������� ��� �������� (��������, A5): ";
	        
	        string input;
	        cin >> input;
	        
	        // ���������, �� ����� �� ������������ ������� ����
	        if (input == "0") {
	            InGameMenu(current_player, enemy, game_over, player1_turn);
	            if (game_over) return false;
	            continue;
	        }
	        
	        // ������������ ������� ���
	        if (input.length() >= 2) {
	            char input_x = input[0];
	            int y;
	            try {
	                y = stoi(input.substr(1));
	            } catch (...) {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            int x;
	            if (input_x >= 'A' && input_x <= 'Z') {
	                x = input_x - 'A';
	            } else if (input_x >= 'a' && input_x <= 'z') {
	                x = input_x - 'a';
	            } else {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            if (x < 0 || x >= current_player.GetCols() || y < 0 || y >= current_player.GetRows()) {
	                InputErrorMessage();
	                extra_turn = true;
	                continue;
	            }
	            
	            int result = enemy.ShotTo(x, y);
	            
	            system("cls");
	            
	            switch (result) {
	                case 0:
	                    cout << "���������! �� ��������� �������������� ���.\n";
	                    extra_turn = true;
	                    break;
	                case 1:
	                    cout << "������� �������! ";
	                    if (enemy.GetCountOfShipsRemain() == 0) {
	                        return true;
	                    }
	                    cout << "���������� ���������� ��������� ��������: " 
	                         << enemy.GetCountOfShipsRemain() << "\n";
	                    extra_turn = true;
	                    break;
	                case 2:
	                    cout << "������!\n";
	                    extra_turn = false;
	                    break;
	                case 3:
	                    cout << "���� ��� ������ ��������!\n";
	                    extra_turn = true;
	                    break;
	                default:
	                    extra_turn = false;
	                    break;
	            }
	            
	            if (extra_turn) {
	                cout << "����������� ��� ���.\n";
	                system("pause");
	            }
	        } else {
	            InputErrorMessage();
	            extra_turn = true;
	        }
	    }
	    
	    return false;
	}
};









/* public ������ SeaBattleBot

*/



class SeaBattleBot : public SeaBattleGame{ //����� �� ���� ������� ���� ��� ����
	private:
		typedef enum BotState{	//��������� ����
			Searching,	//� ������ �������
			Recognition,//������������� ������� ������� (����� � ������� ����� 1 ���)
			Destruction	//����������� �������
		}BotState;
		typedef enum ShipRotation{	//���������� ���������� �������
			Unknown,	//�����������
			Horizontal,	//��������������
			Vertical	//D�����������
		}ShipRotation;
		struct MoveResult{	//��������� ����� �� ����� ����������� �������
			int index;
			int result;
		};
		int ef_cols, ef_rows;	//������ � ������ ����, � ������� ����� ����������� �������
		int LastShotIndex;	//������ ���������� ��� ����
		int LastShotResult;	//��������� ���������� ��� ����
		int solution; //�������, ���� �� ����� ��������
		int DRTN;	//����� (������ ����), ����� �������� ����� �������������� �������� ����������� ���������� ������� (direction recognition turn number)
		BotState State;	//������� ��������� ����
		ShipRotation Rotation;	//���������� ���������� �������
		vector<MoveResult> moves_;	//������ ��������� ����� ��� ����������� �������
		bool ShootingRight; //��� ��� ����������� �������� ������ (����), ����� ����� (�����)
		
	public:
		SeaBattleBot(int enemy_field_cols = 10, int enemy_field_rows = 10) : SeaBattleGame(enemy_field_cols, enemy_field_rows), ef_cols(GetCols()), ef_rows(GetRows()){
			LastShotIndex = -1;
			LastShotResult = -1;
			solution = -1;
			DRTN = -1;
			State = Searching;
			Rotation = Unknown;
			ShootingRight = true;
		}
		void PrintBotMind(){	//�������, ����� ����� �������
			cout << "move_count: " << moves_.size()
			<< "\nLastShotIndex: " << LastShotIndex
			<< "\nLastShotResult: " << LastShotResult
			<< "\nsolution: " << solution
			<< "\nDRTN: " << DRTN
			<< "\nState: " << State
			<< "\nRotation: " << (Rotation == Unknown ? "Unknown" : (Rotation == Horizontal ? "Horizontal" : "Vertical"))
			<< "\nShootingRight: " << ShootingRight
			<< endl;
		}
	public:
		void SetBotFieldSize(int new_cols, int new_rows){	//���������� ������ ����, � ������� ��� ����� ��������
			if (new_cols > 0 && new_cols < 2000000000)
				ef_cols = new_cols;
			else
				ef_cols = GetCols();
			if (new_rows > 0 && new_rows < 2000000000)
				ef_rows = new_rows;
			else
				ef_rows = GetRows();
		}
		void ResetBot(int cols = -1, int rows = -1){
			SetBotFieldSize(cols, rows);
			LastShotIndex = -1;
			LastShotResult = -1;
			solution = -1;
			DRTN = -1;
			State = Searching;
			ResetAboutShipInfo();
			ClearMoves();
			
			
			int ef_cols, ef_rows;	//������ � ������ ����, � ������� ����� ����������� �������
			int LastShotIndex;	//������ ���������� ��� ����
			int LastShotResult;	//��������� ���������� ��� ����
			int solution; //�������, ���� �� ����� ��������
			int DRTN;	//����� (������ ����), ����� �������� ����� �������������� �������� ����������� ���������� ������� (direction recognition turn number)
			BotState State;	//������� ��������� ����
			ShipRotation Rotation;	//���������� ���������� �������
			bool ShootingRight;	//��� ����������� ������� �������� ������
			vector<MoveResult> moves_;	//������ ��������� ����� ��� ����������� �������
		}
	private:
		void ResetAboutShipInfo(){	//�������� ��� ���������� �� ���������� �������
			Rotation = Unknown;
			ShootingRight = true;
		}
		void ClearMoves(){	//�������� ������ ��������� �����
			while(moves_.size() > 0)
				moves_.pop_back();
		}
		void AddMove(int index, int move_result){	//�������� ��������� ����� ���
			MoveResult t_move;
			t_move.index = index;
			t_move.result = move_result;
			moves_.push_back(t_move);
		}
	public:
		void CancelBotMove(){	//�������� ��������� ��� ����
			//CancelLastMove();	//���������� ��������� �� ��������� ���; ����������: 0 - ���� �����������; 1 - ��� ������� �� SHOT, STRIKE, KILL; 2 - ���� ��������� � SHIP; 3 - ��� ������ �������; 4 - ��� ������� �� EMPTY; 5 - ���� ��������� �������;
			
		}
	private:
		void ChangeLastMove(int new_value){
			moves_[moves_.size() - 1].result = new_value;
		}
		void ReverseShootingSide(){	//���������� ����������� ������� �� ��������������� �������
			if (ShootingRight)
				solution = (Rotation == Vertical ? moves_[0].index - ef_cols : moves_[0].index - 1);	//���������� ����� (����) ������ ��������� ������
			else
				solution = (Rotation == Vertical ? moves_[0].index + ef_cols : moves_[0].index + 1);	//���������� ������ (����) ������ ��������� ������
			ShootingRight = !ShootingRight;
		}
		bool IsInField(int index){	//� �������� �� ���� ��� �������� ����������; false - ���; true - ��
			return !(index < 0 || index >= (ef_cols * ef_rows));
		}
		bool IsInFieldHorizontal(int x, int y){	//� �������� �� ���� ��� �������� ����������; false - ���; true - ��
			return !((x < 0 || x >= ef_cols) || (y < 0 || y >= ef_rows));
		}
		bool IsMayShotTo(const SeaBattleField &enemy_field, int index){	//����� �� ����� �������� �� ������� �������; false - ���; true - ��
			return (IsInField(index) && (CheckCell(enemy_field, index) == 0));
		}
		bool IsMayShotToHorizontal(const SeaBattleField &enemy_field, int x, int y){	//����� �� ����� �������� �� ������ ����������; false - ���; true - ��
			return CheckCellHorizontal(enemy_field, x, y) == 0;
		}
		int CheckCell(const SeaBattleField &enemy_field, int index){	//-1 - �� ��������� ����; 0 - ������� (���, ��������, �������); 1 - ��������� ������; 2 - ������� �������; 3 - ���������� �������
			int value = enemy_field.LookAtCellByIndex(index);
			if (enemy_field.GetValueOfValueMean(0) == value)
				return 0;
			else if (enemy_field.GetValueOfValueMean(1) == value)
				return 1;
			else if (enemy_field.GetValueOfValueMean(2) == value)
				return 2;
			else if (enemy_field.GetValueOfValueMean(3) == value)
				return 3;
			else
				return -1;
		}
		int CheckCellHorizontal(const SeaBattleField &enemy_field, int x, int y){
			if (!IsInFieldHorizontal(x, y))
				return -1;
			int index = y * ef_cols + x;
			int value = enemy_field.LookAtCellByIndex(index);
			if (enemy_field.GetValueOfValueMean(0) == value)
				return 0;
			else if (enemy_field.GetValueOfValueMean(1) == value)
				return 1;
			else if (enemy_field.GetValueOfValueMean(2) == value)
				return 2;
			else if (enemy_field.GetValueOfValueMean(3) == value)
				return 3;
			else
				return -1;
		}
		int FindOutShipRotation(const SeaBattleField &enemy_field){	//������ ���������� ���������� �������; 0 - �� �������, 1 - ����� ��� ����������� ���������� ��� ��������; 2 � �������� �����������; 3 - ����� � ��������� ���������� ��� ��������
			int  Up = moves_[0].index - ef_cols, Down = moves_[0].index + ef_cols, Right = (moves_[0].index % ef_cols) + 1, Left = (moves_[0].index % ef_cols) - 1, y = moves_[0].index / ef_cols;	//��������� ��������� ������ � ������ �����������
			cout << "Moves_[0], up, down, left, right " << (moves_[0].index) << Up << Down << Left << Right << endl;
			if (CheckCell(enemy_field, Up) == 2 || CheckCell(enemy_field, Down) == 2){	//���� ���� ��� ���� ��� ����� �������
				Rotation = Vertical;
				cout << "���� ��� ���� ���� ������� �������\n";
				return 1;
			}
			else
				if (!(IsMayShotTo(enemy_field, Up) || IsMayShotTo(enemy_field, Down))){	//����� �� ���������� ����/����
					Rotation = Horizontal;
					cout << "�� ���� ���������� ����/����\n�������� ����� � ����: ";
					cout << CheckCell(enemy_field, Up) << CheckCell(enemy_field, Down) << endl;
					cout << "IsMayShotTo ���� � ���: " << IsMayShotTo(enemy_field, Up) << IsMayShotTo(enemy_field, Down) << endl;
					return 1;
				}
		
			if (CheckCellHorizontal(enemy_field, Left, y) == 2 || CheckCellHorizontal(enemy_field, Right, y) == 2){	//���� ����� ��� ������ ��� ����� �������
				Rotation = Horizontal;
				cout << "����� ��� ������ ���� ������� �������\n";
				return 1;
			}	
			else
				if (!(IsMayShotToHorizontal(enemy_field, Left, y) || IsMayShotToHorizontal(enemy_field, Right, y))){	//����� �� ���������� �����/������
					Rotation = Vertical;
					cout << "�� ���� ���������� �����/������\n";
					return 1;
				}
			
			if (IsMayShotTo(enemy_field, Down)){
				ShootingRight = true;
				solution = Down;
			}
			else if (IsMayShotTo(enemy_field, Up)){
				ShootingRight = false;
				solution = Up;
			}
			else if (IsMayShotToHorizontal(enemy_field, Right, y)){
				ShootingRight = true;
				solution = y * ef_cols + Right;
			}
			else if (IsMayShotToHorizontal(enemy_field, Left, y)){
				solution = y * ef_cols + Left;
				ShootingRight = false;
			}
			
			if (Rotation == Unknown)
				return 2;
			else
				return 3;
		}
		void RecordBotMoveData(int *x = 0, int *y = 0){	//�������� � ���������� ��������� �������� �� solution
			if (x)
				*x = solution % ef_cols;
			if (y)
				*y = solution / ef_cols;
			LastShotIndex = solution;
		}
	public:
		void ShotByBot(SeaBattleField &enemy_field, int *x = 0, int *y = 0){ //���������� ��� ����, ���� ��� ����� ��������; � x � y ����� �������� ���������� ��� ��������;
			int temp = 0, temp2;
			LastShotResult = enemy_field.CheckLastMove();	//0 - ���� �����������; 1 - ��� ������� �� SHOT, STRIKE, KILL; 2 - ���� ��������� � SHIP; 3 - ��� ������ �������; 4 - ��� ������� �� EMPTY; 5 - ���� ��������� �������;
			switch(LastShotResult){	//��������� ��� ���������� ����
				case 2:	//����� � ������ �������
					if (State == Searching){	//������-������ ��������� �������
						AddMove(LastShotIndex, LastShotResult);
						State = Destruction;
					}

					break;
				case 3:	//������� �������
					ResetAboutShipInfo();
					State = Searching;
					break;
				case 4:	//����� �� ������ ������
					
					break;
				/*
				case 5:	//���� ��������� ��������, ��� ������ ���
					solution = 0;
					RecordBotMoveData(x, y);
					return;
				*/
			}
			switch(State){
				case Searching:	//����� ������-���� �������
					
					/*
					//��������. ��� �� ����� ������� �� ������ �� ������ ������
					for(solution = ef_cols * ef_rows - 1; solution >= 0; solution--){
						if (CheckCell(enemy_field, solution) == 0){
							RecordBotMoveData(x, y);
							return;
						}
					}
					break;
					*/
					srand(clock());
					temp = rand();
					srand(clock() + 1);
					solution = (((unsigned int)temp) + rand() % (ef_cols * ef_rows));
					for(int i = 0; i <= ef_cols * ef_rows; solution++, i++){
						if (solution >= ef_cols * ef_rows)
							solution = 0;
						if (CheckCell(enemy_field, solution) == 0)	//����� ���������� � ������ ������
							break;
					}
					RecordBotMoveData(x, y);
					break;
				case Destruction:	//����������� ���������� �������
					if (Rotation == Unknown){	//������� ������ ����������� �������
						temp = FindOutShipRotation(enemy_field);
						if (temp == 1){
							DRTN = moves_.size() - 1;
						}
						else if (temp == 2){
							RecordBotMoveData(x, y);
							AddMove(LastShotIndex, -1);
							return;
						}
						else if (temp == 3){
							RecordBotMoveData(x, y);
							DRTN = moves_.size() - 1;
							return;
						}
						//ChangeLastMove(LastShotResult);
					}
					else
						AddMove(LastShotIndex, LastShotResult);
					
					
					//cout << "LSIndex, solution temp ����� Rotation: " << LastShotIndex << " " << solution << " " << temp << endl;
					
					if (Rotation == Horizontal){
						temp2 = (LastShotResult == 2 ? (LastShotIndex / ef_cols) : (moves_[0].index / ef_cols));
						solution = ((LastShotResult == 2 ? (LastShotIndex % ef_cols) : (moves_[0].index % ef_cols)) + (ShootingRight ? 1 : -1));
					}
					else	//Rotation == Vertical
						solution = ((LastShotResult == 2 ? LastShotIndex : moves_[0].index) + (ShootingRight ? ef_cols : -ef_cols));
					
					
					//cout << "LSIndex, solution temp ����� ���������: " << LastShotIndex << " " << solution << " " << temp << endl;
					//cout << (!IsMayShotToHorizontal(enemy_field, solution, (LastShotResult == 2 ? (LastShotIndex / ef_cols) : (moves_[0].index / ef_cols)))) << endl;
					if ((LastShotResult == 4 && temp == 0) || (Rotation == Horizontal ? (!IsMayShotToHorizontal(enemy_field, solution, temp2)) : (!IsMayShotTo(enemy_field, solution))))	//����� � ������ ������ ��� ��������� ������ ��� ���� ����/�� ����� ������ � �� ��������
						ReverseShootingSide();
					else
						solution = (Rotation == Horizontal ? temp2 * ef_cols + solution : solution);
					
					
					//cout << "LSIndex, solution temp ����� ��������: " << LastShotIndex << " " << solution << " " << temp << endl;
					
					RecordBotMoveData(x, y);
					break;
			}
		}
		void BotSaveToFile(){
			
		}
};







int main() {
	setlocale(LC_ALL, "Rus");
	//��� ������ ������: (���������������)
	//SeaBattleGameMenu menu;
    //menu.Run();
	SeaBattleGame *a, *b, *c;	//���������� a(10, 10), b(10, 10)
	a = new SeaBattleGame;	//���������� a = new SeaBattleGame(10, 10)
	b = new SeaBattleGame(3, 5);	//���������� b = new SeaBattleGame(10, 10)
	c = new SeaBattleGame;
	//SeaBattleField::LoadGameSBF(*a, *b, "test4.txt");	
	//��������� ����
	
	string str1 = "test7.txt";
	c->FullGameTest();
	c->DrawField();
	c->SaveToFile(str1);	//���������� � ����
	b->SaveToFile(str1, true);	//���������� � ����
	//c->LoadFromFile(strt);	//�������� �� �����
	c->DrawField();
	c->PrintChanges();
	
	return 0;
	
	
	printf("\n\n\n\n\n������� ����\n\n\n\n");
	//������� ����. �� ��������
	string str = "text6.bin";
	a->ChangeFieldValueMean(250,  200, 150, 100, 0);
	a->FullGameTest();
	cout << str << endl;
	a->saveToFile(str);	//���������� � ����
	b->saveToFile(str, true);	//���������� � ����
	cout << "�����?\n";
	system("pause");
	a->loadFromFile(str);	//�������� �� �����
	cout << "������ ��������\n";
	b->loadFromFile(str, true);	//�������� �� �����
	//a->SaveToFile(str, true);
	b->ShotTo(2, 2);
	printf("a Shot result: %d\n", a->ShotTo(1, 1));
	//printf("b Shot result: %d\n", b->ShotTo(2, 2));
	printf("b Shot result: %d\n", b->ShotTo(0, 0));
	a->DrawField();
	b->DrawField();
	b->PrintFieldValues();
	a->DebugField();
	b->DebugField();
	
	a->PrintChanges();
	b->PrintChanges();
	
	
	
	return 0;
	/*
	SeaBattleBot c;
	a->SetShipsTest();
	int x = 0, y = 0;
	c.PrintBotMind();
	a->DrawField();
	cout << "\n||||||||||||||||||||||||||||\n\n\n";
	while(a->CheckLastMove() != 1){
		//a->DrawField();
		c.ShotByBot(*a, &x, &y);
		printf("��� ����� �������� � x: %d, y: %d\n", x, y);
		printf("���� ��������: %d\n", a->ShotTo(x, y));
		a->PrintLastChange();
		//c.PrintBotMind();
		cout << "\n||||||||||||||||||||||||||||\n\n\n";
		
		
		
		
	}

	*/
	return 0;
}
