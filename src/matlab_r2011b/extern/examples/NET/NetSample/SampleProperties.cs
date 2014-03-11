using System;
namespace netdoc
{
    public class SampleProperties
    {
        // string property
        private static string stringField = "The MathWorks";
        public static string stringProp
        {
            get { return stringField; }
            set { stringField = value; }
        }

        // read/write property
        double doubleField = 8.9;
        public double doubleProp
        {
            get { return doubleField; }
            set { doubleField = value; }
        }

        // read-only property
        private double readField = 0;
        public double readOnlyProp
        {
            get { return readField; }
        }

        // write-only property
        private double writeField = 0;
        public double writeOnlyProp
        {
            set { writeField = value; }
        }
    }
}
