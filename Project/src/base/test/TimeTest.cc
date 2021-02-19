// #include <iostream>
// #include <boost/date_time/posix_time/ptime.hpp>
// #include <chrono>
// #include <iosfwd>

// #include <stdint.h>
// #include <memory>
// #include <set>
// #include <string>

// using namespace std;

// template <typename C>
// void printClockData ()
// {
//     using namespace std;

//     cout << "- precision: ";
//     // if time unit is less or equal one millisecond
//     typedef typename C::period P;// type of time unit
//     if (ratio_less_equal<P,milli>::value) {
//        // convert to and print as milliseconds
//        typedef typename ratio_multiply<P,kilo>::type TT;
//        cout << fixed << double(TT::num)/TT::den
//             << " milliseconds" << endl;
//     }
//     else {
//         // print as seconds
//         cout << fixed << double(P::num)/P::den << " seconds" << endl;
//     }
//     cout << "- is_steady: " << boolalpha << C::is_steady << endl;
// }

// int main()
// {
//     auto lost_connection_time_ = std::chrono::steady_clock::time_point::min();;
//     cout << to_string(lost_connection_time_) << endl;
// }