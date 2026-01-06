#ifndef MCC_CORE_ERROR_H
#define MCC_CORE_ERROR_H

void mcc_core_error_fatal(const char *format, ...);
void mcc_core_error_report();

#endif  // MCC_CORE_ERROR_H