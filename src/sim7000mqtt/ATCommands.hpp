//
// Created by vadim on 5/26/24.
//

#ifndef TESTSIM7000C_ATCOMMANDS_HPP
#define TESTSIM7000C_ATCOMMANDS_HPP

#define AT "AT"
#define AT_CNACT_ON  AT "+CNACT=1,\"cmnet\""
#define AT_CNACT_OFF AT "+CNACT=0"

#define AT_SMCONF          AT "+SMCONF"
#define AT_SMCONF_URL      AT_SMCONF "=URL,"
#define AT_SMCONF_CLIENTID AT_SMCONF "=CLIENTID,"
#define AT_SMCONF_USERNAME AT_SMCONF "=USERNAME,"
#define AT_SMCONF_PASSWORD AT_SMCONF "=PASSWORD,"

#define AT_SMPUB AT "+SMPUB="

#define AT_SMCONN AT "+SMCONN"
#define AT_SMDISC AT "+SMDISC"

//#define AT_ENDL ""
//#define AT_ENDL "\r\n"
#define AT_ENDL "\r"

#endif //TESTSIM7000C_ATCOMMANDS_HPP
