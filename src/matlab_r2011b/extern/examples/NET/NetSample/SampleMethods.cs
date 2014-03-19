using System;
namespace netdoc
{
    public class SampleMethods
    {
        //static method
        public static double addTwoDoubles(double num1, double num2)
        {
            return num1 + num2;
        }

        //standard method
        public Int32 multiplyInts(Int32 num1, Int32 num2)
        {
            return num1 * num2;
        }

        //test ref and out keyword
        public void refOutTest(ref double db1, out double db2)
        {
            db1 = db1 * 2;
            db2 = db1;
        }

        //test params keyword
        public int paramsTest(params int[] num)
        {
            int total = 0;
            foreach (int i in num)
            {
                total = total + i;
            }
            return total;
        }

    }
}
