using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace ManagedServerForInterop
{
    public interface IComplexNumber
    {
        float AddForRealPartValue {get; set;}
        float AddForImgPartValue {get; set;}
        float AdditionForRealPart(float x1, float x2);
        float AdditionForImgPart(float iy1, float iy2);
        float SubForRealPartValue {get; set;}
        float SubForImgPartValue {get; set;}
        float SubtractionForReadPart(float x1, float x2);
        float SubtractionForImgPart(float iy1, float iy2);
        float MulForRealPartValue {get; set;}
        float MulForImgPartValue {get; set;}
        float MultiplicationForRealPart(float x1, float x2, float y1, float y2);
        float MultiplicationForImgPart(float x1, float iy2, float iy1, float x2);
        float DivForRealPartValue {get; set;}
        float DivForImgPartValue {get; set;}
        float DivisionForRealPart(float x1, float x2, float y1, float y2);
        float DivisionForImgPart(float x1, float x2, float iy1, float iy2);
    }

    [ClassInterface(ClassInterfaceType.AutoDispatch)]
    public class ComplexNumber: IComplexNumber
    {
        public float AddForRealPartValue {get; set;}
        public float AddForImgPartValue {get; set;}
        public float SubForRealPartValue {get; set;}
        public float SubForImgPartValue {get; set;}
        public float MulForRealPartValue {get; set;}
        public float MulForImgPartValue {get; set;}
        public float DivForRealPartValue {get; set;}
        public float DivForImgPartValue {get; set;}

        public ComplexNumber()
        {
            //empty
        }

        public float AdditionForRealPart(float x1, float x2)
        {
            AddForRealPartValue = x1 + x2;
            //MessageBox.Show("Addition for real part is " + AddForRealPartValue);
            return AddForRealPartValue;
        }

        public float AdditionForImgPart(float iy1, float iy2)
        {
            AddForImgPartValue = iy1 + iy2;
            //MessageBox.Show("Addition for img part is " + AddForImgPartValue);
            return AddForImgPartValue;
        }
        public float SubtractionForReadPart(float x1, float x2)
        {
            SubForRealPartValue = x1 - x2;
            //MessageBox.Show("Subtraction for real part is " + SubForRealPartValue);
            return SubForRealPartValue;
        }
        public float SubtractionForImgPart(float iy1, float iy2)
        {
            SubForImgPartValue = iy1 - iy2;
            //MessageBox.Show("Subtraction for img part is " + SubForImgPartValue);
            return SubForImgPartValue;
        }
        public float MultiplicationForRealPart(float x1, float x2, float y1, float y2)
        {
            MulForRealPartValue = (x1*x2)-(y1*y2);
            //MessageBox.Show("Multiplication for real part is " + MulForRealPartValue);
            return MulForRealPartValue;
        }
        public float MultiplicationForImgPart(float x1, float iy2, float iy1, float x2)
        {
            MulForImgPartValue = (x1*iy2)+(iy1*x2);
            //MessageBox.Show("Multiplication for img part is " + MulForImgPartValue);
            return MulForImgPartValue;
        }
        public float DivisionForRealPart(float x1, float x2, float y1, float y2)
        {
            float realNumerator = ((x1*x2) + (y1*y2));
            float realDenominator = (((x2)*(x2)) + (((y2)*(y2)))); 
            DivForRealPartValue = (realNumerator/realDenominator);
            //MessageBox.Show("Division for real part is " + DivForRealPartValue);
            return DivForRealPartValue;
        }
        public float DivisionForImgPart(float x1, float x2, float iy1, float iy2)
        {
            float imgNumerator = (((iy1)*(x2))-((x1)-(iy2)));
            float imgDenominator = (((x2)*(x2)) + ((iy2)*(iy2)));
            DivForImgPartValue = (imgNumerator/imgDenominator);
            //MessageBox.Show("Division for img part is " + DivForImgPartValue);
            return DivForImgPartValue;
        }
    }
}

