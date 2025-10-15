#include <iostream>
#include <cstdlib>	
#include <locale>	
//#include "Field.h"	//Библиотека "Поле игры 'Морской бой'"
#include <vector>	//Для сохранения и отката ходов
#include <cstring>	//Для работы с файлами
#include <fstream>	//Для работы с файлами
#include <iomanip> //Для форматирования текста
#include <windows.h> // для изменения цвета текста
#include <time.h> // для использования генератора случайных чисел

using namespace std;

class SeaBattleField;


class SeaBattleField{	//Механика и логика
		//Обозначения: EMPTY, SHOT, STRIKE, KILL, SHIP
	private:	//Значения
		struct CellValueChange{	//Хранение изменения состояния клетки при ходе
		    int coordinate_index;	//Индекс клетки
		    unsigned char prevState;	//Состояние клетки до изменения
		    unsigned char newState;		//Состояние клетки после изменения
		};
		int cols, rows;	//Допустимы значения от 1 до 100 миллионов, но cols * rows до 1 миллиарда
		unsigned char *field;	//Допустимы значения от 0 до 255
		unsigned char *field_value_mean;	//Допустимы значения от 0 до 255
		unsigned char *ships;	//ships[0] - максимальная длина корабля, ships[от 1 до 255] - кол-во кораблей. Макс. кол-во кораблей любой длины - 255
		unsigned char *ships_remain;	//копия ships, однако ships_remain является оставшимся кол-вом кораблей в игре, а не настройкой игры
	
		vector<vector<CellValueChange> > moves;	//Хранение ходов
		
	
		//Конструкторы и перегрузки операторов
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
			printf("Дестр.\n");
			if (field)
				delete[] field;
			if (field_value_mean)
				delete[] field_value_mean;
			if (ships)
				delete[] ships;
			if (ships_remain)
				delete[] ships_remain;
			printf("Дестр уничтожил.\n");
		}
		SeaBattleField(const SeaBattleField& other){
			printf("Начало Конструктор копирования\n\n");
			field = 0;
			field_value_mean = 0;
			ships = 0;
			ships_remain = 0;
			cols = 1;
			rows = 1;
			printf("cols в начале = %d, rows = %d\n", cols, rows);
			field = new unsigned char[1];
			field_value_mean = new unsigned char[5];
			ships = new unsigned char[1];
			ships_remain = new unsigned char[1];
			if (!field || !field_value_mean || !ships || !ships_remain)
				MemoryAllocationError(0);
			/*Устанавливает все значения "по умолчанию"
			ClearField();
			ResetFieldValueMean();
			ResetShips();
			ResetShipsRemain();
			*/
			printf("Нач\n");
			printf("other.cols = %d, %p\n\n\n", other.cols, &other.cols);
			printf("this fvm0 = %d %d\n", field_value_mean[0], field_value_mean[1]);
				printf("111\n");
	        printf("Итог ChangeFieldSize: %d\n", ChangeFieldSize(other.cols, other.rows));
			ChangeFieldValueMean(other.field_value_mean[0], other.field_value_mean[1], other.field_value_mean[2], other.field_value_mean[3], other.field_value_mean[4]);
				printf("222\n");
	        for(int i = cols * rows - 1; i >= 0; i--)
	            field[i] = other.field[i];
	            printf("333\n");
	        printf("Итог ChangeMaxShipLen: %d\n", ChangeMaxShipLen(other.ships[0]));
	            printf("444\n");
			for(int i = 1; i <= other.ships[0]; i++)
				printf("Итог ChangeShipLenCount: %d\n", ChangeShipLenCount(i, other.ships[i]));
				printf("555\n");
			moves = other.moves;
			printf("cols = %d, rows = %d\n", cols, rows);
			for(int i = 0; i < rows; i++, printf("\n"))
				for(int n = 0; n < cols; n++)
					printf("%d ", field[cols * i + n]);
			printf("Конец Конструктор копирования\n\n");
		}
		SeaBattleField &operator=(const SeaBattleField& other){
	        printf("Начало Оператор присваивания\n\n");
			if (this != &other) {
	            ChangeFieldValueMean(other.field_value_mean[0], other.field_value_mean[1], other.field_value_mean[2], other.field_value_mean[3], other.field_value_mean[4]);
	            printf("1\n");
				printf("Итог ChangeFieldSize: %d\n", ChangeFieldSize(other.cols, other.rows));
	            printf("2\n");
				for(int i = cols * rows - 1; i >= 0; i--)
	            	field[i] = other.field[i];
	            printf("3\n");
	            printf("Итог ChangeMaxShipLen: %d\n", ChangeMaxShipLen(other.ships[0]));
	            printf("4\n");
				for(int i = 1; i <= other.ships[0]; i++)
					printf("Итог ChangeShipLenCount: %d\n", ChangeShipLenCount(i, other.ships[i]));
				printf("5\n");
				moves = other.moves;
				printf("cols = %d, rows = %d\n", cols, rows);
				for(int i = 0; i < rows; i++, printf("\n"))
					for(int n = 0; n < cols; n++)
						printf("%d ", field[cols * i + n]);
				
	            // Копируем данные из другого объекта
	        }
	        printf("Конец Оператор присваивания\n\n");
	        return *this; // Возвращаем текущий объект
	    }
		
		
		//Получение хранящихся значений
	public:
		int GetRows(){	//Возвращает значение rows
			return rows;
		}
		int GetCols(){	//Возвращает значение cols
			return cols;
		}
		int GetValueOfValueMean(int type_position){	//Если type_position[0, 4] - вернётся текущее значение типа, иначе вернётся 1111 (невозможное значение)
			if (type_position < 0 || type_position > 4)
				return 1111;
			return field_value_mean[type_position];
		}
		int GetShipsRemainCountOfNLen(int n){	//[1, (ship_max_len - 1)] - количество кораблей; -1 - ошибка длины корабля
			if (n < 1 || n > ships[0])
				return -1;
			return ships_remain[n]; // возврат кол-ва оставшихся кораблей длины n
		}
		int GetMaxShipLen(){	//Возвращает длину самого большого корабля
			return ships[0];
		}
		int GetCountOfShips(){	//Возвращает сумму кораблей, которое будет в игре
			int sum = 0;
			for(int i = 1; i <= ships[0]; i++)
				sum += ships[i];
			return sum;
		}
		int GetCountOfShipsRemain(){	//Возвращает сумму оставшихся кораблей в игре
			int sum = 0;
			for(int i = 1; i <= ships_remain[0]; i++)
				sum += ships_remain[i];
			return sum;
		}
		int GetCellsOfShipsSum(){	//Возвращает сумму клеток кораблей в которое будет в игре
			int sum = 0;
			for(int i = 1; i <= ships[0]; i++)
				sum += (ships[i] * i);
			return sum;
		}
		int GetCellsOfShipsRemainSum(){	//Возвращает сумму клеток не взорванных кораблей, оставшееся в игре
			int sum = 0;
			for(int i = cols * rows - 1; i >= 0; i--)
				if (field[i] == field_value_mean[4] || field[i] == field_value_mean[2])
					sum += 1;
			return sum;
		}
	protected:
		int GetValueOfCellByIndex(int index){	//Возвращает хранящееся значение в клетке, иначе -1
			if (index < 0 || index >= rows * cols)
				return -1;
			return field[index];
		}
		int GetValueOfCell(int col_index, int row_index){	//Возвращает хранящееся значение в клетке, иначе -1
			if ((col_index < 0 || row_index < 0) || (col_index >= cols || row_index >= rows))
				return -1;
			return field[row_index * rows + col_index];
		}
		
		
		//Сбросы значений до стандартных
	public:
		void ClearField(){	//Каждая клетка принимает значение EMPTY этого поля
			for(int i = cols * rows - 1; i >= 0; i--)
				field[i] = field_value_mean[0];
		}
		void ResetFieldValueMean(){	//Значения типов клеток поля становятся стандартными (EMPTY - 0, SHOT - 1, ..., SHIP - 4)
			for(int i = 0; i < 5; i++)
				field_value_mean[i] = i;
		}
		void ResetShips(){	//Количество кораблей становится равно 4 1-палубных, ..., 1 4-палубный
			unsigned char *t = 0;
			t = new unsigned char[5];
			if (!t)
				MemoryAllocationError(1);
			else
			{
				if (ships)
					delete[] ships;
				ships = t;
				ships[0] = 4;	//Макс. длина
				ships[1] = 4;
				ships[2] = 3;
				ships[3] = 2;
				ships[4] = 1;
			}
		}
		void ResetShipsRemain(){	//Значения ships_remain становятся равны значениям ships
			for(int i = 0; i <= ships[0]; i++)
				ships_remain[i] = ships[i];
		}
		void ResetField(){	//Удалить все сделанные ходы
			ClearField();
			while(moves.size() > 0)
				moves.pop_back();
		}
		
		//Изменения значений для игры
	public:
		int ChangeFieldValueMean(int EMPTY = 0, int SHOT = 1, int STRIKE = 2, int KILL = 3, int SHIP = 4){	//0 - успешно; 1 - ошибка, одинаковые значения; 2 - недопустимое значение (<0 или >255)
			if (IsInCharRange(EMPTY) || IsInCharRange(SHOT) || IsInCharRange(STRIKE) || IsInCharRange(KILL) || IsInCharRange(SHIP))
				return 2;
			char temp[5] = {(char)EMPTY, (char)SHOT, (char)STRIKE, (char)KILL, (char)SHIP};
			for(int i = 0; i < 5; i++)	//Проверяется, если 2 разных типа клетки поля совпадают значением
				for(int n = 0; n < 5; n++)
					if (i != n)
						if (temp[i] == temp[n])
							return 1;
			for(int i = cols * rows - 1; i >= 0; i--)	//Замена старых значений типов в клетках на новые
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
		int ChangeFieldSize(int new_cols_count, int new_rows_count){	//0 - успешно; 1 - ошибка выделения памяти; 2 - некорректные размеры поля
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
		int ChangeShipLenCount(int ship_len, int new_ship_count){	//Изменяет максимум возможных кораблей определённой длины. 0 - успешно; 1 - ошибка значений
			if ((ship_len < 1 || ship_len > ships[0]) || (new_ship_count < 0 || new_ship_count > 255))
				return 1;
			ships[ship_len] = new_ship_count;
			ships_remain[ship_len] = new_ship_count;
			return 0;
		}
		int ChangeMaxShipLen(int new_max_len){	//Изменяет максимальную возможную длину корабля. 0 - успешно; 1 - ошибка выделения памяти; 2 - недопустимая максимальная длина
			if (new_max_len < 1 || new_max_len > 255)
				return 1;
			unsigned char *temp = 0;
			temp = new unsigned char[new_max_len + 1];
			if (!temp)
				return MemoryAllocationError(1);
			temp[0] = new_max_len;
			for(int i = 1; i <= temp[0]; i++)	//Копирование в новый массив данных о кораблях и запись в новые ячейки 0
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
		
		
		//Работа с сохраняемыми ходами
	private:
		void MakeMove(){	//Создаёт новый ход
			vector<CellValueChange> move;
			moves.push_back(move);
		}
		void AddCellValueChange(int index, unsigned char prev_value, unsigned char new_value){	//Добавляет в последний ход изменение ячейки
			CellValueChange change = {index, prev_value, new_value};
        	moves[moves.size() - 1].push_back(change);
		}
		void TransformByIndexValueChange(int index, unsigned char prev_value, unsigned char new_value){	//Ищет в последнем ходе по index клетку и меняет prev и new значения на новые
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
		int CancelLastMove(){	//Откатывает изменения за последний ход; Возвращает: 0 - ходы отсутствуют; 1 - был выстрел по SHOT, STRIKE, KILL; 2 - было попадание в SHIP; 3 - был подрыв корабля; 4 - был выстрел по EMPTY; 5 - была установка корабля;
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
	        		if (moves[moves.size() - 1][i].newState == field_value_mean[4])	//Стала SHIP
	        			result = 5;
					else if (moves[moves.size() - 1][i].newState == field_value_mean[2])	//Стала STRIKE
						result = 2;
					else if (moves[moves.size() - 1][i].prevState == field_value_mean[4])	//Была SHIP
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
		int CheckLastMove(){ //"Подсмотреть", что делал последний ход; Возвращает то же, что и CancelLastMove()
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
		
		
		//Чтение из файла
	public:
		static int LoadGameSBF(SeaBattleField &field_1, SeaBattleField &field_2, const char *file_name){	//Загрузка из файла; те же, что и в LoadFromFile
			ifstream file(file_name);	//Открываем файл для чтения
			int t1 = field_1.LoadFromFile(file);
			if (t1)
				return t1;
			int t2 = field_2.LoadFromFile(file);
			if (t2)
				return t2;
			return 0;
		}
	private:
		int LoadFromFile(ifstream &read_file){	//Загрузка из файла; 0 - успешно; 1 - ошибка открытия файла / выделения памяти; 2 - ошибка считывания данных; 3 - ошибка значения данных в файле
			//ifstream read_file(char_file_name);	//Открываем файл для чтения
			if (read_file.is_open())
			{
		        int a, b;
		        
		        //Считывание cols и rows
			    if (!(read_file >> a) || !(read_file >> b))           
					return ReadFileError(read_file, 2);
				
				if (ReadIsStringEnd(read_file) || ChangeFieldSize(a, b))
					return ReadFileError(read_file, 3);
				
				//Считывание значений field
				a = ReadArray(read_file, field, 0, 0, cols * rows);
				if (a)
					return ReadFileError(read_file, a);
					
				//Считывание значений field_value_mean
				if (!(read_file.get() == 'f'))
					return ReadFileError(read_file, 2);
				printf("  field_value_mean\n");
				a = ReadArray(read_file, field_value_mean, 0, 0, 5);
				if (a)
					return ReadFileError(read_file, a);
					
				//Считывание значений ships
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
					
				//Считывание значений ships_remain
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
				//Считывание значений ходов
				if (!(read_file >> b))           
					return ReadFileError(read_file, 2);
				if (ReadIsStringEnd(read_file))
					return ReadFileError(read_file, 3);
				for(int i = 0; i < b; i++)
				{
					printf("Шаг %d\n", i);
					if (!(read_file.get() == 'm') || !(read_file >> a))
						return ReadFileError(read_file, 2);
					a = ReadMove(read_file, a);
					if (a)
						return ReadFileError(read_file, a);
				}
				printf("Начало проверок.\n Первый этап\n");
					//Проверка возможности существования такого расклада игры
				//1) Корректное ли на поле количество кораблей
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
				a = 0;	//клеток кораблей на поле
				b = 0;	//живых клеток кораблей на поле
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
					printf("Насчитал: %d кораблей, а надо: %d\n", a, GetCellsOfShipsSum());
					return ReadFileError(read_file, 3);
				}
				printf(" Второй этап\n");
				//2) Соответствуют ли ходы тому, во что превратится field
				for(int i = 0; i < moves.size(); i++)	//Какое поле получилось из ходов
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
				for(int i = cols * rows - 1; i >= 0; i--)	//Сравнение загруженного и восстановленного из ходов полей
					if (field[i] != temp_field[i])
					{
						printf("ERROR: index = %d, fi[] = %d, te[] = %d\n", i, field[i], temp_field[i]);
						delete[] temp_field;
						return ReadFileError(read_file, 3);
					}
				delete[] temp_field;
				printf("  Загрузка успешна\n\n\n");
		    }
		    read_file.close();	// Закрываем файл
		}
		int ReadFileError(ifstream &flow_name, int error_type){	//Закрывает поток и возвращает номер ошибки
			flow_name.close();
			printf("ReadFileError(code = %d)!\n", error_type);
			return error_type;
		}
		int ReadIsStringEnd(ifstream &flow_name){	//Проверяет на окончание строку и переходит на следующую; 0 - строка закончилась; 1 - строка не закончилась
			if (flow_name.get() == '\n')
				return 0;
			return 1;
		}
		int ReadICRNumber(ifstream &flow_name, int *number){	//Считывание из файла числа; 0 - успешно; 1 - ошибка считывания; 2 - ошибка, за пределами значений char
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
		int ReadArray(ifstream &flow_name, unsigned char *array, int add_to_index, int start_from, int max_elements){	//Запись значений из файла в массив; 0 - успешно; 1 - ошибка окончания файла; 2 - ошибка считывания данных; 3 - ошибка значения данных в файле
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
				printf("Строка не закончилась, ошибка.\n");
				return 2;
			}
			if (start_from < max_elements)
				return 1;
			return 0;
		}
		int ReadMove(ifstream &flow_name, int size){	//Считывание из файла одного хода; 0 - успешно; 1 - ошибка окончания файла; 2 - ошибка считывания данных; 3 - ошибка значения данных в файле
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
				if (i > 0 && (pr_st == field_value_mean[0] && ne_st == field_value_mean[4]))	//Ставится ли корабль непрерывной линией
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
				printf("Строка не закончилась, ошибка.\n");
				return 2;
			}
			if (flow_name.eof())
				return 1;
			return 0;
		}
		
		//Запись в файл
	public:
		static int SaveGameSBF(SeaBattleField &field_1, SeaBattleField &field_2, const char *file_name){	//Сохранение в файл; те же, что и в SaveToFile
			ofstream file(file_name);	//Открываем файл для записи
			field_1.SaveToFile(file);
			field_2.SaveToFile(file);
		}
	private:
		int SaveToFile(ofstream &record_file){	//Сохранение в файл; 0 - успешно; 1 - ошибка открытия файла; 2X - ошибка записи (X - код ошибки)
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
				cout << "Не удалось открыть файл для записи\n";
				return 1;
			}
			record_file.close();
			return 0;
		}
		int RecordFileError(ofstream &flow_name, int error_type){	//Закрывает поток и возвращает номер ошибки
			flow_name.close();
			printf("RecordFileError(code = %d)!\n", error_type);
			return error_type;
		}
		int RecordItem(ofstream &flow_name, int value, string set_after_item){	//0 - успешно; 1 - ошибка записи
			if (value == -1)
				if (!(flow_name << set_after_item))
					return 1;
				else
					return 0;
			if (!(flow_name << value) || !(flow_name << set_after_item))
				return 1;
			return 0;
		}
		int RecordLine(ofstream &flow_name, unsigned char *array, int count){	//0 - успешно; 1 - ошибка записи
			for(int i = 0; i < count - 1; i++)
				if (RecordItem(flow_name, array[i], " "))
					return 1;
			if (RecordItem(flow_name, array[count - 1], "\n"))
				return 1;
			return 0;
		}
		int RecordMove(ofstream &flow_name, int moves_index, int move_index){	//0 - успешно; 1 - ошибка записи
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
		int RecordMoves(ofstream &flow_name){									//0 - успешно; 1 - ошибка записи
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
		
		
		//Работа с кораблями
	private:
		void SetInSide(unsigned char set_what, int side, int len, int x, int y){	//ставит set_what, len клеток в направлении side
			int x_2 = XYChangesBySide(0, side), y_2 = XYChangesBySide(1, side), index;
			for(side = 0, x = x - x_2, y = y - y_2; side < len; x = x + x_2, y = y + y_2, side++)
			{
				index = cols * (y + y_2) + (x + x_2);
				AddCellValueChange(index, field[index], set_what);
				field[index] = set_what;
			}	
		}
		int CheckOneBlock(int x, int y){	//0 - всё в порядке / координата за пределами поля; >0 - кол-во кораблей
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
		int CheckSide(int side, int len, int x, int y){	//0 - всё в порядке; 1 - за пределами поля / корабли
		    int x_2 = XYChangesBySide(0, side), y_2 = XYChangesBySide(1, side);
			for(side = 0, len--, x += x_2, y += y_2; side < len; x += x_2, y += y_2, side++)
				if ((x < 0 || x >= cols) || (y < 0 || y >= rows) || CheckOneBlock(x, y))
					return 1;
			return 0;
		}
		int FindShipEdge(int *x, int *y){	//Возвращает направление корабля и в *x и *y запишет координаты крайней клетки корабля
			int index = cols * *y + *x;
			if ((*x + 1 < cols && (field[index + 1] == field_value_mean[4] || field[index + 1] == field_value_mean[3] || field[index + 1] == field_value_mean[2])) ||
        		  (*x - 1 >= 0 && (field[index - 1] == field_value_mean[4] || field[index - 1] == field_value_mean[3] || field[index - 1] == field_value_mean[2])))
			{
				*x += (CalculateWhatInSide(*x, *y, 2, -1) - 1) * XYChangesBySide(0, 2);	//горизонтальное расположение корабля
				return 4;
			}
			*y += (CalculateWhatInSide(*x, *y, 1, -1) - 1) * XYChangesBySide(1, 1);
			return 3;
		}
		int CalculateWhatInSide(int x, int y, int side, int what){	//считает количество what в направлении side до EMPTY или SHOT клетки
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
		int SetShotAroundShips(int side, int len, int x, int y){	//возвращает итоговое кол-во уст. SHOT
			int i = 0;
			i += ShotToWallOfSide(side, x, y, 1);
			i += ShotToMiddleOfSide(side, x, y, len);
			x += (len - 1) * XYChangesBySide(0, side);
			y += (len - 1) * XYChangesBySide(1, side);
			i += ShotToWallOfSide(side, x, y, 0);
			return i;
		}
		int ShotToMiddleOfSide(int side, int x, int y, int len){	//возвращает количество установленных SHOT
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
		int ShotToWallOfSide(int side, int x, int y, int back){	//возвращает количество установленных SHOT
			if (back)	//чтобы отзеркалить от side сторону для установки SHOT
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
		int ShotTo(int x, int y){	//Выстрелить; 0 - попадание по SHIP; 1 - подрыв последнего корабля; 2 - выстрел в пустую клетку; 3 - бессмысленный выстрел, попадание в SHOT, STRIKE, KILL; 4 - ошибка координаты; 5 - ошибка значения ячейки
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
				side = FindShipEdge(&x_, &y_);	//После функции x_ и y_ становятся координатой крайней (правой / верхней) клетки корабля
				len = CalculateWhatInSide(x_, y_, side, -1);
				if (CalculateWhatInSide(x_, y_, side, field_value_mean[2]) == len)	//True - подрыв, False - попадание
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
		
		int SetShip(int ship_len, int side, int x, int y){ //Установить корабль; 0 - успешно; 1 - ошибка аргумента; 2 - ошибка установки первой клетки; 3 - ошибка установки в направлении
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
			if (!((AvailableSides(x, y, ship_len) / IntPow(10, 4 - side))%10))	//проверяется доступность установки в сторону side
				return 3;
			MakeMove();
			SetInSide(field_value_mean[4], side, ship_len, x, y);
			return 0;
		}
		
		int AvailableSides(int x, int y, int ship_len){ //возвращает доступные стороны закодированным числом (верх - 1, право - 10, низ - 100, лево - 1000)
		    int side, sum = 0;
		    if (((x < 0 || x >= cols) || (y < 0 || y >= rows)) || CheckOneBlock(x, y))
		    	return 0;
		    for(side = 1; side < 5; side++)
				if (!CheckSide(side, ship_len, x, y))
					sum = sum + IntPow(10, 4 - side);
			return sum;
		}
	
		
		//Вычисления для методов
	private:
		int IsInCharRange(int number){	//0 - в диапазоне; 1 - за диапазоном
			return (number < 0 || number > 255) ? 1 : 0;
		}
		int IsNewSizeAvailable(int cols_, int rows_){	//0 - доступно; 1 - недоступно
			if ((cols_ < 1 || cols_ > 100000000) || (rows_ < 1 || rows_ > 100000000) || (cols_ > 1000000000 / rows_))
				return 1;
			return 0;
		}
		int MemoryAllocationError(int error_type){	//Значения error_type: 0 - критически важный объект; 1 - не критически важный объект
			switch(error_type){
				case 0:
					printf("exit 1;\n");
					exit(1);
				case 1:
					return 1;
					break;
			}
		}
		int XYChangesBySide(int x_is_0_y_is_1, int side){	//значения изменения x, y в зависимости от стороны
		    if (x_is_0_y_is_1)	//в зависимости от направления x = 0, 1, 0, -1;    y = 1, 0, -1, 0
		    	return (side < 3) ? 0 - side%2 : (side == 3) ? 1 : 0;
			return (side%2) ? 0 : (side == 2) ? 1 : -1;
		}
		int IntPow(int x, int y){	//Возвращает x в степени y
			int a = 1;
			for(y; y > 0; y--)
					a *= x;
			return a;
		}
		int IsOneOfFieldValueMean(unsigned char number){	//0 - является каким-либо значением из field_value_mean; 1 - не является
			if (number == field_value_mean[0] || number == field_value_mean[1] || number == field_value_mean[2] || number == field_value_mean[3] || number == field_value_mean[4])
				return 0;
			return 1;
		}
		
		
		//Для отрисовки
	public:				//ВРЕМЕННО СУЩЕСТВУЮТ МЕТОДЫ!!!!!!!!!!!!!!!!!!!
		void PrintChanges(){	//Отобразить все ходы и изменения, происходящие в них
			for(int n = 0, moves_count = (moves.size() - 1); n <= moves_count; n++)
			{
				printf("    moves[%d]\n", n);
				for (int i = 0, size = (moves[n].size() - 1); i <= size; i++){
	            	printf("index = %d, prev = %d, new = %d\n", moves[n][i].coordinate_index, moves[n][i].prevState, moves[n][i].newState);
	        	}
			}
		}
		void DebugField(){	//   УДАЛИТЬ 
			printf("Object: %p\n", this);
			printf("cols: %p\nrows: %p\n", &cols, &rows);
			printf("field: %p\nfield_value_mean: %p\n", field, field_value_mean);
			printf("ships: %p\nships_remain: %p\n", ships, ships_remain);
			printf("moves: %p\n", &moves);
		}
};




class SeaBattleGame : public SeaBattleField //Интерфейс и реализация игры
{	
	private:
		char *FieldSymbol;	//символы отображения клеток во время игры
	public:
		SeaBattleGame(int cols_ = 10, int rows_ = 10) : SeaBattleField(cols_, rows_) {
			FieldSymbol = 0;
			FieldSymbol = new char[5];
			if (!FieldSymbol)
				exit(1);
			FieldSymbol[0] = '-';	//Пустая клетка
			FieldSymbol[1] = '*';	//Стреленая клетка
			FieldSymbol[2] = 'X';	//Раненая клетка корабля
			FieldSymbol[3] = '#';	//Клетка взорванного корабля
			FieldSymbol[4] = 'S';	//Целая клетка корабля
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
			FieldSymbol[0] = other.FieldSymbol[0];	//Пустая клетка
			FieldSymbol[1] = other.FieldSymbol[1];	//Стреленая клетка
			FieldSymbol[2] = other.FieldSymbol[2];	//Раненая клетка корабля
			FieldSymbol[3] = other.FieldSymbol[3];	//Клетка взорванного корабля
			FieldSymbol[4] = other.FieldSymbol[4];	//Целая клетка корабля
		}
		void DrawSymbol(unsigned char index){	//Отрисовка элемента index поля, в виде игрового символа
			cout << FieldSymbol[GetValueOfCellByIndex(index)];
		}
		
		void DrawFields(SeaBattleGame &enemy_field) 
		{
	        int y;
	        
	        PrintWordNTimes(" ", GetCols() - 1);
	        cout << "Ваше поле ";
	        PrintWordNTimes(" ", 2 * GetCols() + 3);
	        cout << "Поле врага" << endl;
	        
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
		
	    void draw_row(int y, int owner) //строки с учётом владения полем
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
		
		void DrawField(){	//Отрисовать 1 поле игровыми символами
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
		{						//буквенная полоса горизонтальных координат
			short i;
			for(printf("   "), i = 0; i < this->GetCols(); i++)
				printf(" %c", i + 'A');
		}
		
		void PrintWordNTimes(string word, int n){ 				//вывести строку n раз
			for(n; n > 0; n--)
				cout << word;
		}
		
		void PrintFieldValues(){	//Отрисовать 1 поле хранимыми значениями
			for(int index = 0; index < GetCols() * GetRows(); index++){
				cout << GetValueOfCellByIndex(index);
				if (((index + 1) % GetCols() == 0) && (index > 0))
					cout << endl;
				else
					cout << " ";
			}
			cout << endl;
		}
		void PrintFieldIndexs(){	//Отрисовать индексы 1 поля
			for(int index = 0; index < GetCols() * GetRows(); index++){
				cout << index;
				if (((index + 1) % GetCols() == 0) && (index > 0))
					cout << endl;
				else
					cout << " ";
			}
		}
		void PrintFieldValueMean(){	//Отобразить значения, которыми обозначаются EMPTY, SHOT, ..., SHIP
			cout << "Обозначения:\nEMPTY - " << GetValueOfValueMean(0) << "\nSHOT - " << GetValueOfValueMean(1)
			<< "\nSTRIKE - " << GetValueOfValueMean(2) << "\nKILL - " << GetValueOfValueMean(3)
			<< "\nSHIP - " << GetValueOfValueMean(4) << "\n";
		}
		
		void SetShipsRandomly() 
		{
	        ClearField();
	        ResetShipsRemain();
	        srand(time(NULL)); // инициализация генератора
	        
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
	            int x = rand() % GetCols(); // Случайная координата X
	            int y = rand() % GetRows(); // Случайная координата Y
	            
	            if (len == 1) {
	            	// Для 1-палубного корабля
	                if (SetShip(1, 1, x, y) == 0) {
	                    return true;
	                }
	            } else {
	            	// Для многопалубных кораблей
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
};


//Надо сделать методы:
//SeaBattleField:
//	
//SeaBattleGame:
//	DrawColored();	//Отрисовать строку цветом
//	DrawFields();	//Чтобы отрисовывалось два поля по горизонтали. Над полями и слева от каждого из них выводятся координаты (сверху буквы, слева цифры).
//	Menu(); //Начать новую игру, загрузить имеющуюся игру, изменить параметры игры (перекинет в Settings()), завершить программу
//	Settings(); //Позволит отсюда изменить размер игрового поля и количество кораблей, которое будет в игре (перед подтверждением изменений выводить предупреждение, т.к. данные будут очищены. После изменения хотя бы одного параметра приводить значения к стандартным (очистить поле, удалить сделанные ходы, ships_remain приравнять к ships)
//	GetReadyForGame(); //Сделать все параметры готовыми для игры (очистка поля, сброс ходов и участвовавших кораблей в игре).
//	Game(SeaBattleField &field_2); либо static Game(SeaBattleField &field_1, SeaBattleField &field_2);	//Для проведения игры. В ней возможность делать ходы, откатывать ход и т.д.
//	Seed(); //Возвращает "случайное" число, используемое для генераций чего-либо в программе
//	SetShipsRandomly(); //Расставить корабли "случайным" образом
//	
//SeaBattleBot() //Это будет класс со всей логикой бота
//	//Помнит итог последнего хода
//	
//	BotMakeMove(); //Сделать ход
//	ShotLogic(); //Вычисления, куда стрелять
	

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
        cout << "Добро пожаловать в игру 'Морской бой'.\n";
        MainMenu();
    }

private:
    void MainMenu() {
        int choice = -1;
        
        while (true) {
            cout << "Вы находитесь в главном меню. Введите номер того действия, которое хотите выполнить.\n";
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
        cout << "\t" << NEW_GAME << " Начать новую игру\n";
        cout << "\t" << CHANGE_SHIPS << " Изменить количество кораблей\n";
        cout << "\t" << RESET_SHIPS_COUNT << " Сбросить количество кораблей\n";
        cout << "\t" << CHANGE_MAX_SHIP_LEN << " Изменить максимальную длину корабля в игре\n";
        cout << "\t" << INSTRUCTION << " Инструкция по игре\n";
        cout << "\t" << EXIT << " Выйти\n";
    }
    
    void NewGame() {
        int choice = -1;
        
        while (true) {
            cout << "Выберите режим игры:\n";
            cout << "\t" << PLAYER_VS_PLAYER << " Человек с человеком\n";
            cout << "\t" << BACK_TO_MENU << " Назад в меню\n";
            
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
        
        player1_color = InputColorID("Первого игрока");
        player2_color = InputColorID("Второго игрока");
        
        cout << "Сейчас очередь ";
        PrintColored("Первого игрока ", player1_color);
        cout << "расставлять корабли.\n";
        
        if (Confirm("Расставить корабли вручную?")) {
            SetShipsManually(*player1);
        } else {
            SetShipsRandomly(*player1);
        }
        
        system("cls");
        cout << "Ваши корабли успешно установлены.\nТеперь очередь ";
        PrintColored("Второго игрока ", player2_color);
        cout << "расставлять корабли.\n";
        
        if (Confirm("Расставить корабли вручную?")) {
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
        cout << "Режим ручной расстановки кораблей\n";
        game.DrawField();
        
        int ship_len = game.GetMaxShipLen();
        while (ship_len > 0) {
            int count = game.GetShipsRemainCountOfNLen(ship_len);
            for (int i = 0; i < count; i++) {
                system("cls");
                cout << "Расстановка " << ship_len << "-палубного корабля (" << (i+1) << "/" << count << ")\n";
                game.DrawField();
                
                if (ship_len == 1) {
                    int x, y;
                    cout << "Введите координаты для 1-палубного корабля: ";
                    while (!GetShotCoordinates(x, y) || !game.SetShip(1, 1, x, y)) {
                        InputErrorMessage();
                        cout << "Введите координаты для 1-палубного корабля: ";
                    }
                } 
				else 
				{
                    int x, y, side;
                    cout << "Введите координаты начальной точки " << ship_len << "-палубного корабля: ";
                    while (!GetShotCoordinates(x, y)) {
                        InputErrorMessage();
                        cout << "Введите координаты начальной точки " << ship_len << "-палубного корабля: ";
                    }
                    
                    int available_sides = game.AvailableSides(x, y, ship_len);
                    if (available_sides == 0) {
                        cout << "Из этой точки нельзя установить корабль. Попробуйте другую точку.\n";
                        i--;
                        continue;
                    }
                    
                    cout << "Доступные направления: ";
                    if ((available_sides / 1000) % 10) cout << "1(Вверх) ";
                    if ((available_sides / 100) % 10) cout << "2(Вправо) ";
                    if ((available_sides / 10) % 10) cout << "3(Вниз) ";
                    if (available_sides % 10) cout << "4(Влево) ";
                    cout << "\nВыберите направление: ";
                    
                    while (!(cin >> side) || side < 1 || side > 4 || !((available_sides / IntPow(10, 4-side)) % 10)) {
                        InputErrorMessage();
                        cout << "Выберите направление: ";
                    }
                    
                    if (game.SetShip(ship_len, side, x, y) != 0) {
                        cout << "Не удалось установить корабль. Попробуйте снова.\n";
                        i--;
                    }
                }
            }
            ship_len--;
        }
        
        system("cls");
        cout << "Все корабли успешно расставлены!\n";
        game.DrawField();
        system("pause");
    }
    
    void SetShipsRandomly(SeaBattleGame& game) {
        game.SetShipsRandomly(); // метод с таким же названием, но в классе SeaBattleGame
        cout << "Корабли расставлены случайным образом.\n";
        game.DrawField();
        
        if (!Confirm("Такое расположение подходит?")) {
            SetShipsRandomly(game);
        }
    }
    
    void StartGame() {
        system("cls");
        cout << "Все корабли успешно расставлены.\n";
        cout << "Начинается Морской бой. Приятной игры!\n";
        system("pause");
        
        bool game_over = false;
        bool player1_turn = true;
        
        while (!game_over) 
		{
            if (!player1_turn) {
                cout << "\aСмена очереди хода!\n";
                cout << "Сейчас будет ходить ";
                PrintColored("Второй игрок", player2_color);
                cout << ".\n";
                system("pause");
                system("cls");
            }
            
            bool turn_result;
	        if (player1_turn) {
	            turn_result = PlayerTurn(*player1, *player2, "Первого игрока", player1_color, player2_color, game_over, player1_turn);
	        } else {
	            turn_result = PlayerTurn(*player2, *player1, "Второго игрока", player2_color, player1_color, game_over, player1_turn);
	        }
	        
	        if (turn_result) {
	            game_over = true;
	        } 
			else if (!game_over) { // переход хода другому игроку
	            player1_turn = !player1_turn;
	        }
	        
	        if (!game_over) {
            	system("pause");
        	}
        }
        
        system("cls");
        cout << "\aВсе корабли ";
        if (player1_turn) {
            PrintColored("Второго игрока ", player2_color);
        } else {
            PrintColored("Первого игрока ", player1_color);
        }
        cout << "потоплены!\nПобеду одержал ";
        if (player1_turn) {
            PrintColored("Первый игрок", player1_color);
        } else {
            PrintColored("Второй игрок", player2_color);
        }
        cout << ".\nСпасибо за игру!\n";
        system("pause");
        system("cls");
    }
    
    bool GetShotCoordinates(int& x, int& y) {
        char input_x;
        cout << "Введите координаты для выстрела (например, A5): ";
        
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
        cout << "Изменение количества кораблей:\n";
        PrintShipsCount(ships_count_main);
        
        int ship_len, new_count;
        cout << "Введите длину корабля для изменения: ";
        while (!(cin >> ship_len) || ship_len < 1 || ship_len >= ship_len_limit) {
            InputErrorMessage();
            cout << "Введите длину корабля для изменения: ";
        }
        
        int current_count = (ships_count_main / IntPow(10, ship_len-1)) % 10;
        cout << "Текущее количество " << ship_len << "-палубных кораблей: " << current_count << "\n";
        cout << "Введите новое количество: ";
        
        while (!(cin >> new_count) || new_count < 0 || new_count > 9) {
            InputErrorMessage();
            cout << "Введите новое количество: ";
        }
        
        ships_count_main = ships_count_main - (current_count * IntPow(10, ship_len-1)) + (new_count * IntPow(10, ship_len-1));
        
        cout << "Количество кораблей изменено.\n";
        PrintShipsCount(ships_count_main);
        system("pause");
        system("cls");
    }
    
    void ResetShipsCount() {
        if (Confirm("Сбросить количество кораблей к стандартным значениям?")) {
            ships_count_main = 1234;
            ship_len_limit = 5;
            cout << "Значения сброшены к стандартным.\n";
        } else {
            cout << "Значения не были изменены.\n";
        }
        system("pause");
        system("cls");
    }
    
    void ChangeMaxShipLen() {
        int new_max_len;
        cout << "Текущая максимальная длина корабля: " << (ship_len_limit - 1) << "\n";
        cout << "Введите новую максимальную длину (1-9): ";
        
        while (!(cin >> new_max_len) || new_max_len < 1 || new_max_len > 9) {
            InputErrorMessage();
            cout << "Введите новую максимальную длину (1-9): ";
        }
        
        if (Confirm("Сохранить изменение?")) {
            ship_len_limit = new_max_len + 1;
            
            if (ship_len_limit < 5) {
                ships_count_main = ships_count_main % IntPow(10, ship_len_limit - 1);
            }
            
            cout << "Максимальная длина изменена на " << new_max_len << ".\n";
        } else {
            cout << "Изменения отменены.\n";
        }
        
        system("pause");
        system("cls");
    }
    
    void PrintInstruction() {
        cout << "///////////////////////////////////ИНСТРУКЦИЯ///////////////////////////////////\n\n";
        cout << "Игра 'Морской бой'\n\n";
        cout << "Цель игры: первым уничтожить все корабли противника.\n\n";
        cout << "Правила:\n";
        cout << "- Игровое поле размером 10x10 клеток\n";
        cout << "- Корабли расставляются вертикально или горизонтально\n";
        cout << "- Корабли не могут соприкасаться друг с другом\n";
        cout << "- При попадании в корабль дается дополнительный ход\n";
        cout << "- При уничтожении корабля помечаются клетки вокруг него\n\n";
        cout << "Символы на поле:\n";
        cout << "'-' - пустая клетка\n";
        cout << "'*' - промах\n"; 
        cout << "'X' - раненый корабль\n";
        cout << "'#' - уничтоженный корабль\n";
        cout << "'S' - ваш корабль (виден только на вашем поле)\n\n";
        cout << "Управление:\n";
        cout << "- Для выстрела вводите букву и цифру (например: A5)\n";
        cout << "- Буквы от A до J - горизонтальные координаты\n";
        cout << "- Цифры от 0 до 9 - вертикальные координаты\n";
        cout << "////////////////////////////////КОНЕЦ ИНСТРУКЦИИ////////////////////////////////\n\n";
        system("pause");
        system("cls");
    }
    
    bool Exit() {
        if (Confirm("Вы точно хотите выйти?")) {
            return true;
        }
        system("cls");
        return false;
    }
    
    void InputErrorMessage() {
        system("cls");
        cout << "Неправильный ввод! Попробуйте ещё раз.\n\n";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    
    bool Confirm(const string& message) {
        int choice;
        while (true) {
            cout << message << "\n'0' - если да.\n'1' - если нет.\n";
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
            cout << "Доступные цвета для " << player_name << ":\n";
            cout << "1 - Красный\n2 - Зеленый\n3 - Желтый\n4 - Синий\n";
            cout << "5 - Фиолетовый\n6 - Голубой\n7 - Белый\n";
            cout << "Выберите цвет (1-7): ";
            
            if (cin >> color && color >= 1 && color <= 7) {
                cout << "Пример: ";
                PrintColored(player_name, color);
                cout << "\n";
                
                if (Confirm("Этот цвет подходит?")) {
                    return color;
                }
            } else {
                InputErrorMessage();
            }
        }
    }
    
    void PrintShipsCount(int ships_count) {
        cout << "Корабли в игре:\n";
        for (int n = ship_len_limit - 1; n > 0; n--) {
            int count = (ships_count / IntPow(10, n-1)) % 10;
            if (count > 0) {
                cout << "  " << count << " x " << n << "-палубных\n";
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
            
            cout << "\n=== ПОДМЕНЮ ИГРЫ ===\n";
            cout << "1 - Продолжить игру\n";
            cout << "2 - Сохранить игру\n";
            cout << "3 - Загрузить другую игру\n";
            cout << "4 - Выйти в главное меню\n";
            cout << "5 - Завершить программу\n";
            cout << "Выберите действие: ";
            
            if (!(cin >> choice) || choice < 1 || choice > 5) {
                InputErrorMessage();
                continue;
            }
            
            switch (choice) {
                case 1:
                    // Продолжить игру
                    return;
                    
                case 2:
                    // Сохранить игру
                    SaveCurrentGame(current_player, enemy, player1_turn);
                    break;
                    
                case 3:
                    // Загрузить другую игру
                    if (LoadDifferentGame(current_player, enemy, player1_turn)) {
                        game_over = false;
                        return;
                    }
                    break;
                    
                case 4:
                    // Выйти в главное меню
                    if (Confirm("Вы точно хотите выйти в главное меню? Текущий прогресс будет потерян.")) {
                        game_over = true;
                        return;
                    }
                    break;
                    
                case 5:
                    // Завершить программу
                    if (Confirm("Вы точно хотите завершить программу?")) {
                        exit(0);
                    }
                    break;
            }
        }
    }
    
    void SaveCurrentGame(SeaBattleGame& player1, SeaBattleGame& player2, bool current_turn) {
        string filename;
        cout << "Введите имя файла для сохранения (без пробелов): ";
        cin >> filename;
        
        // Добавляем расширение если нужно
        if (filename.length() < 4 || filename.substr(filename.length() - 4) != ".txt") 
		{
		    filename += ".txt";
		}
        
        // Сохраняем информацию о текущем ходе
        ofstream info_file("game_info.txt");
        if (info_file.is_open()) {
            info_file << current_turn << endl;
            info_file.close();
        }
        
        // Сохраняем саму игру
        if (SeaBattleField::SaveGameSBF(player1, player2, filename.c_str()) == 0) // c_str - возвращает указатель на char (const char*)
		{
            cout << "Игра успешно сохранена в файл: " << filename << endl;
        } else {
            cout << "Ошибка при сохранении игры!" << endl;
        }
        
        system("pause");
    }
    
    bool LoadDifferentGame(SeaBattleGame& player1, SeaBattleGame& player2, bool& current_turn) {
        string filename;
        cout << "Введите имя файла для загрузки: ";
        cin >> filename;
        
        // Добавляем расширение если нужно
        if (filename.find(".txt") == string::npos) {
            filename += ".txt";
        }
        
        // Загружаем игру
        if (SeaBattleField::LoadGameSBF(player1, player2, filename.c_str()) == 0) {
            // Загружаем информацию о ходе
            ifstream info_file("game_info.txt");
            if (info_file.is_open()) {
                info_file >> current_turn;
                info_file.close();
            }
            
            cout << "Игра успешно загружена из файла: " << filename << endl;
            system("pause");
            return true;
        } else {
            cout << "Ошибка при загрузке игры!" << endl;
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
	        
	        cout << "Ход ";
	        PrintColored(player_name, player_color);
	        cout << "\n";
	        
	      
	        cout << "Введите '0' для вызова меню или координаты для выстрела (например, A5): ";
	        
	        string input;
	        cin >> input;
	        
	        // Проверяем, не хочет ли пользователь вызвать меню
	        if (input == "0") {
	            InGameMenu(current_player, enemy, game_over, player1_turn);
	            if (game_over) return false;
	            continue;
	        }
	        
	        // Обрабатываем обычный ход
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
	                    cout << "Попадание! Вы получаете дополнительный ход.\n";
	                    extra_turn = true;
	                    break;
	                case 1:
	                    cout << "Корабль взорван! ";
	                    if (enemy.GetCountOfShipsRemain() == 0) {
	                        return true;
	                    }
	                    cout << "Оставшееся количество вражеских кораблей: " 
	                         << enemy.GetCountOfShipsRemain() << "\n";
	                    extra_turn = true;
	                    break;
	                case 2:
	                    cout << "Промах!\n";
	                    extra_turn = false;
	                    break;
	                case 3:
	                    cout << "Туда нет смысла стрелять!\n";
	                    extra_turn = true;
	                    break;
	                default:
	                    extra_turn = false;
	                    break;
	            }
	            
	            if (extra_turn) {
	                cout << "Продолжайте ваш ход.\n";
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


int main() {
	setlocale(LC_ALL, "Rus");
	
	SeaBattleGameMenu menu;
    menu.Run();
	

	return 0;
}
