#ifndef MATRIX_H
#define MATRIX_H

namespace SGDK
{

class MatrixException
{
private:
	int numError;
public:
	MatrixException(int i);
	const wchar_t* Error() const;
};
class SquareMatrix
{
public:

	double **M;
	int size;

SquareMatrix(int inSize);

virtual ~SquareMatrix();

    void Kramer(double free_members_column[], double koeff[]);   // Метод Крамера решения систем линейных уравнений

double&			operator	()	(int row, int col) const;
SquareMatrix	operator	*	(const double &f);
SquareMatrix	operator	!	();
SquareMatrix&	operator	=	(const double&);

double determinant;
double determinant_dop;
//double determinant2;
//double determinant3;

double Determinant() const;
SquareMatrix Allied();
SquareMatrix PreMinor(int row, int col) const;
void Print();
};

}  //namespace SGDK


#endif // MATRIX_H
