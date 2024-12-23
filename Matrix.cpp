#include "Matrix.h"
#include <iostream>

//using namespace std;

namespace SGDK
{


MatrixException::MatrixException(int i)
{
	numError = i;
}
const wchar_t* MatrixException::Error() const
{ 
	static const wchar_t *arrayErrors[]  =  
	{
	/*  0  */	L"Неизвестная ошибка.",  
	/*  1  */	L"Переполнение. Попытка доступа к несуществующему элементу.",    
	/*  2  */	L"Обратная матрица не рассчитана: определитель равен 0.",  
	/*  3  */	L"Невозможно рассчитать минор для матрицы данного размера.",  
	/*  4  */	L"Неверный адрес переменной.",
	/*  5  */	L"Ошибка ввода размера матрицы.", 
	/*  6  */	L"Невозможно выделить память.", 
	/*  7  */	L"Пункта меню с таким номером нет.", 
	/*  8  */	L"Размер матрицы должен задаваться целым числом и быть больше нуля.",  
	/*  9  */	L"Невозможно присвоить значение элементу. Проверьте вводимое значение.",
	/*  10 */	L"Невозможно открыть файл для записи.",
	/*  11 */	L"Невозможно открыть файл для чтения.",
	/*  12 */	L"Количество чисел в файле меньше необходимого.",
	/*  13 */	L"Нет строки с таким номером.",
	/*  14 */	L"Нет столбца с таким номером."
	};  
	
	if (numError>=0 && numError<=15) {
		return arrayErrors[numError]; 
	}
	else {
		return 0;
	}
}
SquareMatrix::SquareMatrix(int inSize)
{
    size = inSize;
    M = (double**) new double[size];
    for(int i = 0; i < size; i++)
    {
        M[i] = (double*) new double[size];
        for(int j = 0; j < size; j++)
        {
            M[i][j] = 0;
        }
    }
}

SquareMatrix::~SquareMatrix()
{
    // Удаление матрицы из памяти
    for(int i = 0; i < size; i++)
    {
        delete M[i];
    }
    delete M;
}


double& SquareMatrix::operator()(int row, int col) const
{ 
	if (row >= size || col >= size) {
		throw MatrixException(1);
	}
	return (double&) M[row][col];	
 }
SquareMatrix SquareMatrix::operator*(const double &f)
{	
	if (f==0) {
		throw MatrixException(4);
	}
	for(int i = 0,j; i<size; i++) {
		for(j = 0; j<size; j++) {
			(*this)(i, j) *= f;
		}
	}
	return *this;
}
SquareMatrix SquareMatrix::operator!()
{ 
	SquareMatrix newMatrix(size);
	int i,j;

	for(i = 0; i<size; i++) {
		for(j = 0; j<size; j++) {
			newMatrix(j, i) = (*this)(i, j);
		}
	}
	return newMatrix;
}
SquareMatrix& SquareMatrix::operator=(const double &f)
{ 
	if (f==0) {
		throw MatrixException(4);
	}
	for(int i = 0,j; i<size; i++) {
		for(j = 0; j<size; j++) {
			if(i == j) {
				(*this)(i, j) = f;
			}
			else {
				(*this)(i, j) = 0;
			}
		}
	}
	return *this;
}
double SquareMatrix::Determinant() const {
    double determ = 0;
    if (size == 1) {
		return (*this)(0, 0);
	}
	for(int i = 0; i<size; i++) {
		double a = (*this)(0, i) * (i%2 ? -1 : 1);
		determ += a * this->PreMinor(0, i).Determinant();
	}        
/*   упрощенный способ для матрицы 3х3
    determ = (*this)(0,0)*(*this)(1,1)*(*this)(2,2)
            -(*this)(0,0)*(*this)(2,1)*(*this)(1,2)
            -(*this)(0,1)*(*this)(1,0)*(*this)(2,2)
            +(*this)(0,1)*(*this)(2,0)*(*this)(1,2)
            +(*this)(0,2)*(*this)(1,0)*(*this)(2,1)
            -(*this)(0,2)*(*this)(2,0)*(*this)(1,1);
*/
	return determ;
}
SquareMatrix SquareMatrix::Allied() {
	if ((size-1)<=0) {
		throw MatrixException(3);
	}
	SquareMatrix newMatrix(size);
	int i,j;

	for(i = 0; i<size; i++) {
		for(j = 0; j<size; j++) {
			newMatrix(i,j) = this->PreMinor(i, j).Determinant() * ((i+j)%2 ? -1 : 1);
		}
	}
	return newMatrix;
}
SquareMatrix SquareMatrix::PreMinor(int row, int col) const
{
	SquareMatrix newMatrix(size-1);
	int i, j, in, jn; 

	for(i = 0, in = 0; i<size; i++) {
		if (i != row){
			for(j = 0, jn = 0; j<size; j++) {
				if (j != col) {
					newMatrix(in, jn++)= (*this)(i, j);
				}
			}
			in++;
		}
	}
    return newMatrix;
}

void SquareMatrix::Kramer(double free_members_column[], double koeff[])   // Метод Крамера решения систем линейных уравнений
{
    // нахожу определитель
    determinant = Determinant();

    double TempArr[size];


    for (int i=0; i<size; i++)
    {
        // Заменяю i-й столбец на массив свободных членов
        for (int j=0; j<size; j++)
        {
            TempArr[j] = (*this)(j, i);
            (*this)(j, i) = free_members_column[j];
        }

        double determinant_dopol = Determinant();

        //возвращаю i-й столбец назад
        for (int j=0; j<size; j++)
        {
            (*this)(j, i) = TempArr[j];
        }

        koeff[i] = determinant_dopol/determinant;

    }
}

/*void SquareMatrix::Print()
{
	int i,j;     
	//for (i=0; i<size; i++) {
	//		cout << " ---------------";
	//}
	//cout << endl;
	cout.precision(3);
	cout.setf(ios::showpoint | ios::fixed | ios::left);
	for (i=0 ; i<size; i++) {
		for (j=0; j<size; j++) {
			//cout << "|" << " [" << i+1 << "," << j+1 << "]" << "\t" << (*this).M[i][j] << "\t";
			cout << "[" << i+1 << "," << j+1 << "]" << " ";
			cout.width(12);
			cout << (*this).M[i][j];
		}
		//cout << "|" << endl;
		//for (j=0;j<size;j++) {
		//	cout << " ---------------";
		//}
		cout << endl;
	}
}*/

} // namespace SGDK
