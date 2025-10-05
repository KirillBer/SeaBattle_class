#include <iostream>
#include <cstdlib>	
#include <locale>	
//#include "Field.h"	//���������� "���� ���� '������� ���'"
#include <vector>	//��� ���������� � ������ �����
#include <cstring>	//��� ������ � �������
#include <fstream>	//��� ������ � �������

using namespace std;

class SeaBattleField;



class SeaBattleField{	//�������� � ������
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
		int GetValueOfValueMean(int type_position){	//���� type_position[0, 4] - ������� ������� �������� ����, ����� ������� 1111 (����������� ��������)
			if (type_position < 0 || type_position > 4)
				return 1111;
			return field_value_mean[type_position];
		}
		int GetShipsRemainCountOfNLen(int n){	//[1, (ship_max_len - 1)] - ���������� ��������; -1 - ������ ����� �������
			if (n < 1 || n > ships[0])
				return -1;
			return ships_remain[n];
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
					else if (moves[moves.size() - 1][i].prevState == field_value_mean[4])	//���� SHIP
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
		int CheckLastMove(){ //"�����������", ��� ����� ��������� ���; ���������� �� ��, ��� � CancelLastMove()
			if (!moves.size())
				return 0;
			if (!moves[moves.size() - 1].size())
				return 1;
			if (moves[moves.size() - 1][0].newState == field_value_mean[4])
	        	return 5;
			else if (moves[moves.size() - 1][0].newState == field_value_mean[2])
				return 2;
			else if (moves[moves.size() - 1][0].prevState == field_value_mean[4])
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
		static int SaveGameSBF(SeaBattleField &field_1, SeaBattleField &field_2, const char *file_name){	//���������� � ����; �� ��, ��� � � SaveToFile
			ofstream file(file_name);	//��������� ���� ��� ������
			field_1.SaveToFile(file);
			field_2.SaveToFile(file);
		}
	private:
		int SaveToFile(ofstream &record_file){	//���������� � ����; 0 - �������; 1 - ������ �������� �����; 2X - ������ ������ (X - ��� ������)
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
		void PrintChanges(){	//���������� ��� ���� � ���������, ������������ � ���
			for(int n = 0, moves_count = (moves.size() - 1); n <= moves_count; n++)
			{
				printf("    moves[%d]\n", n);
				for (int i = 0, size = (moves[n].size() - 1); i <= size; i++){
	            	printf("index = %d, prev = %d, new = %d\n", moves[n][i].coordinate_index, moves[n][i].prevState, moves[n][i].newState);
	        	}
			}
		}
		void DebugField(){	//   ������� 
			printf("Object: %p\n", this);
			printf("cols: %p\nrows: %p\n", &cols, &rows);
			printf("field: %p\nfield_value_mean: %p\n", field, field_value_mean);
			printf("ships: %p\nships_remain: %p\n", ships, ships_remain);
			printf("moves: %p\n", &moves);
		}
};




class SeaBattleGame : public SeaBattleField{	//��������� � ���������� ����
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
		void DrawSymbol(unsigned char index){	//��������� �������� index ����, � ���� �������� �������
			cout << FieldSymbol[GetValueOfCellByIndex(index)];
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
		void DrawFields(){	//���������� 2 ������� ����
			
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
		void FullGameTest(){	//�������� ���� �� ����� ����
			SetShip(4, 2, 0, 0);
			DrawField();
			SetShip(3, 2, 0, 2);
			DrawField();
			SetShip(3, 3, 5, 0);
			DrawField();
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

/*
(��� ����� ��������� ���������� ����� ���� �������, ���� ������ � ����� �������)
public ������ �:

	SeaBattleField:
		//��������� ��������
		int GetRows(){	//���������� �������� rows
		int GetCols(){	//���������� �������� cols
		int GetValueOfCellByIndex(int index){	//���������� ���������� �������� � ������, ����� -1
		int GetValueOfCell(int col_index, int row_index){	//���������� ���������� �������� � ������, ����� -1
		int GetValueOfValueMean(int type_position){	//���� type_position[0, 4] - ������� ������� �������� ����, ����� ������� 1111 (����������� ��������)
		int GetShipsRemainCountOfNLen(int n){	//[1, (ship_max_len - 1)] - ���������� ��������; -1 - ������ ����� �������
		int GetMaxShipLen(){	//���������� ����� ������ �������� ������� � ships
		int GetCountOfShips(){	//���������� ����� ��������, ������� ����� � ����
		int GetCountOfShipsRemain(){	//���������� ����� ���������� �������� � ����
		int GetCellsOfShipsSum(){	//���������� ����� ������ �������� � ������� ����� � ����
		int GetCellsOfShipsRemainSum(){	//���������� ����� ������ �� ���������� ��������, ���������� � ����
		
		//������ ��������
		void ClearField(){	//������ ������ ��������� �������� EMPTY ����� ����
		void ResetFieldValueMean(){	//�������� ����� ������ ���� ���������� ������������ (EMPTY - 0, SHOT - 1, ..., SHIP - 4)
		void ResetShips(){	//���������� �������� ���������� ����� 4 1-��������, 3 2-��������, ..., 1 4-��������
		void ResetShipsRemain(){	//�������� ships_remain ���������� ����� ��������� ships
		void ResetField(){	//������� ��� ��������� ����
		
		//��������� ��������
		int ChangeFieldValueMean(int EMPTY = 0, int SHOT = 1, int STRIKE = 2, int KILL = 3, int SHIP = 4){	//0 - �������; 1 - ������, ���������� ��������; 2 - ������������ �������� (<0 ��� >255)
		int ChangeFieldSize(int new_cols_count, int new_rows_count){	//0 - �������; 1 - ������ ��������� ������; 2 - ������������ ������� ����
		int ChangeShipLenCount(int ship_len, int new_ship_count){	//�������� �������� ��������� �������� ����������� �����. 0 - �������; 1 - ������ ��������
		int ChangeMaxShipLen(int new_max_len){	//�������� ������������ ��������� ����� �������. 0 - �������; 1 - ������ ��������� ������; 2 - ������������ ������������ �����
		
		//������ � ���������
		int ShotTo(int x, int y){	//����������; 0 - ��������� �� SHIP; 1 - ������ ���������� �������; 2 - ������� � ������ ������; 3 - ������������� �������, ��������� � SHOT, STRIKE, KILL; 4 - ������ ����������; 5 - ������ �������� ������
		int SetShip(int ship_len, int side, int x, int y){ //0 - �������; 1 - ������ ���������; 2 - ������ ��������� ������ ������; 3 - ������ ��������� � �����������
		int AvailableSides(int x, int y, int ship_len){ //���������� ��������� ������� �������������� ������ (���� - 1, ����� - 10, ��� - 100, ���� - 1000)
		
		//���������� � �������� (����)
		static int LoadGameSBF(SeaBattleField &field_1, SeaBattleField &field_2, const char *file_name){	//�������� �� �����; �� ��, ��� � � LoadFromFile (0 - �������, �� 0 - ������)
		static int SaveGameSBF(SeaBattleField &field_1, SeaBattleField &field_2, const char *file_name){	//���������� � ����; �� ��, ��� � � SaveToFile (0 - �������, �� 0 - ������)
		
		//����
		int CancelLastMove(){	//���������� ��������� �� ��������� ���; ����������: 0 - ���� �����������; 1 - ��� ������� �� SHOT, STRIKE, KILL; 2 - ���� ��������� � SHIP; 3 - ��� ������ �������; 4 - ��� ������� �� EMPTY; 5 - ���� ��������� �������;
		int CheckLastMove(){ //"�����������", ��� ����� ��������� ���; ���������� �� ��, ��� � CancelLastMove()


	SeaBattleGame:
		void DrawSymbol(unsigned char index){	//��������� �������� index ����, � ���� �������� �������



��������� ����������:
<�����, � ������� ���� �������� ������>
	<�����, ������� ���� �������>
		<������ �������, � ������� ������� ��� ����� �����������>


���� ������� ������:
SeaBattleField:
	
SeaBattleGame:
	DrawColored();	//���������� ������ ������
	DrawFields();	//����� �������������� ��� ���� �� �����������. ��� ������ � ����� �� ������� �� ��� ��������� ���������� (������ �����, ����� �����).
	Menu(); //������ ����� ����, ��������� ��������� ����, �������� ��������� ���� (��������� � Settings()), ��������� ���������
	Settings(); //�������� ������ �������� ������ �������� ���� � ���������� ��������, ������� ����� � ���� (����� �������������� ��������� �������� ��������������, �.�. ������ ����� �������. ����� ��������� ���� �� ������ ��������� ��������� �������� � ����������� (�������� ����, ������� ��������� ����, ships_remain ���������� � ships)
	GetReadyForGame(); //������� ��� ��������� �������� ��� ���� (������� ����, ����� ����� � ������������� �������� � ����).
	Game(SeaBattleField &field_2); ���� static Game(SeaBattleField &field_1, SeaBattleField &field_2);	//��� ���������� ����. � ��� ����������� ������ ����, ���������� ��� � �.�.
	Seed(); //���������� "���������" �����, ������������ ��� ��������� ����-���� � ���������
	SetShipsRandomly(); //���������� ������� "���������" �������
	
SeaBattleBot() //��� ����� ����� �� ���� ������� ����
	//������ ���� ���������� ����
	
	BotMakeMove(); //������� ���
	ShotLogic(); //����������, ���� ��������
	
	

(��� �������� ������ ����� ���, � ����� ������. ���� ����� � ��������� ������� ������.

*/

int main() {
	setlocale(LC_ALL, "Rus");
	SeaBattleGame *a, *b;	//���������� a(10, 10), b(10, 10)
	a = new SeaBattleGame;	//���������� a = new SeaBattleGame(10, 10)
	b = new SeaBattleGame;	//���������� b = new SeaBattleGame(10, 10)
	//SeaBattleGame::LoadGameSBF(*a, *b, "test4.txt");	//�������� �� �����

	return 0;
}
