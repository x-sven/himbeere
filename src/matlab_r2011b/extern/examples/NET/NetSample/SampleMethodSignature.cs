using System;
namespace netdoc
{
    public class SampleMethodSignature
    {
        public SampleMethodSignature()
        { }

        public SampleMethodSignature(double d)
        { myDoubleField = d; }

        public SampleMethodSignature(string s)
        { myStringField = s; }

        public int myMethod(string strIn, ref double dbRef,
                            out double dbOut)
        {
            dbRef += dbRef;
            dbOut = 65;
            return 42;
        }

        private Double myDoubleField = 5.5;
        private String myStringField = "hello";
    }
}
