#include <boost/test/unit_test.hpp>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>

using namespace std;

void test_case_stream_logging_parser()
{

    unsigned int counter = 0;

    ostringstream filebase, logfile;
    filebase << "logfile_sleep_tester.log";
    logfile << "_" << counter << "_" << filebase.str().c_str();
    ifstream input(logfile.str().c_str(), ifstream::in);

    while(input && input.good())
    {
        vector<int> v_iTime;
        vector<int> v_iSleep;
        vector<float> v_fCPU;

        cout << "=============================================" << endl
             << "Testing log file: " << logfile.str().c_str()   << endl
             << "=============================================" << endl;

        string header;
        getline(input, header); // read headline

        int iTime, iSleep;
        float fCPU;
        while(input >> iTime >> iSleep >> fCPU)
        {
            v_iTime.push_back(iTime);
            v_iSleep.push_back(iSleep);
            v_fCPU.push_back(fCPU);
        }
#if(DEBUG)
        cout << "number of lines: " << v_iTime.size() << endl;
#endif
        BOOST_CHECK_GT(v_iTime.size(), 0);

        vector<int> v_dT;
        for(unsigned ii=0; ii < v_iTime.size()-1; ii++) //skip first line comment
        {
            v_dT.push_back(v_iTime.at(ii+1) - v_iTime.at(ii));
        }
        vector<int>::iterator  itr_max_dT =    max_element( v_dT.begin(), v_dT.end() );
        vector<int>::iterator  itr_min_dT =    min_element( v_dT.begin(), v_dT.end() );
#if(DEBUG)
        vector<float>::iterator  itr_max_CPU = max_element( v_fCPU.begin(), v_fCPU.end() );
        cout << "Maximum element in Vector v_fCPU is: " << *itr_max_CPU << endl;
        cout << "Maximum element in Vector v_dT is: " << *itr_max_dT << endl;
#endif

        // CPU
        double sum = accumulate(v_fCPU.begin(), v_fCPU.end(), 0.0);
        double meanCPU = sum / v_fCPU.size();

        // Delta T
        sum = accumulate(v_dT.begin(), v_dT.end(), 0.0);
        double mean_dT = sum / v_dT.size();
        vector<double> diff(v_dT.size());
        transform(v_dT.begin(), v_dT.end(), diff.begin(),
                       bind2nd(minus<double>(), mean_dT));
        double sq_sum = inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        double stdev = sqrt(sq_sum / v_dT.size());

        // Checks
        BOOST_CHECK_GE( meanCPU, 0.2); //%
        BOOST_CHECK_LE( meanCPU, 0.6); //%
        BOOST_CHECK_GE( mean_dT, 9.5); //ms
        BOOST_CHECK_LE( mean_dT, 10.5); //ms
        BOOST_CHECK_GE( *itr_min_dT, 5);   //ms
        BOOST_CHECK_LE( *itr_max_dT, 15);  //ms
        BOOST_CHECK_LE( stdev, 2); //ms

//#if(DEBUG)
        cout << "mean CPU: "       << meanCPU << endl;
        cout << "mean dT: "        << mean_dT << endl;
        cout << "dT std. dev.: "   << stdev   << endl;
        cout << "max(dT): "        << *itr_max_dT<< endl;
        cout << "min(dT): "        << *itr_min_dT<< endl;

//#endif
        input.close();
        logfile.str("");
        logfile << "_" << ++counter << "_" << filebase.str().c_str();
        input.open(logfile.str().c_str(), ifstream::in);
    } //while
}
