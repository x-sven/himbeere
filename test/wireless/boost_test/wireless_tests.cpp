#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>
#include <ostream>
#include <inttypes.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/algorithm/string/find.hpp>


using namespace std;


//!< First read this: http://www.boost.org/doc/libs/1_53_0/libs/test/doc/html/utf/user-guide.html

//!< The following define is used to access privat methods
//!< without modifications of the included classes.
#define private public


BOOST_AUTO_TEST_CASE( proc_find_test )
{

    const string IFNAME = "wlan";


    ifstream input("/proc/net/wireless");
    if(!input)
    {
        //cout << "Couldn't open the file\n";
       BOOST_FAIL( "Couldn't open the file");
    }

    string line;
    while(getline(input, line))
    {
        istringstream bp(line);
        string fname;
        bp >> fname;
        if( boost::algorithm::find_first(fname, IFNAME) )
        {
            replace(line.begin(), line.end(), '.', ' ');
            cout << "first number: " << fname << '\n';
            string word;
            while(bp >> word)
                cout << "next number: " << word << '\n';
        }
    }


}


