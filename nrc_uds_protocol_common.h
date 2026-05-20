#ifndef NRC_UDS_PROTOCOL_COMMON_H
#define NRC_UDS_PROTOCOL_COMMON_H


 #define UDS_NRC_generalReject                                      0x10 //Very general. Sent in case none of the other NRC's can describe the error
 #define UDS_NRC_serviceNotSupported                                0x11 //ECU does not support the requested service
 #define UDS_NRC_subFunctionNotSupported                            0x12 //Requested sub-function is not supported
 #define UDS_NRC_incorrectMessageLengthOrInvalidFormat              0x13 //Specified length of the message or fortmat is invalid
 #define UDS_NRC_responseTooLong                                    0x14 //The response length exceeds the limits specified by the underlying layer
 #define UDS_NRC_busyRepeatRequest                                  0x21 //ECU is busy to perform the required operation
 #define UDS_NRC_conditionsNotCorrect                               0x22 //Pre-requisites to perform the UDS request are not valid, so the request cannot go through
 #define UDS_NRC_requestSequenceError                               0x24 //When a certain sequence required for an UDS request is not complied with
 #define UDS_NRC_requestOutOfRange                                  0x31 //When a parameter value is outside the valid range
 #define UDS_NRC_securityAccessDenied                               0x33 //Request denied due to the security measures established to perform the request by the ECU are not complied with
 #define UDS_NRC_invalidKey                                         0x35//Security access denied due to invalid key
 #define UDS_NRC_exceedNumberOfAttempts                             0x36 //Request will not be executed because there were more than N failed security access attempts
 #define UDS_NRC_requiredTimeDelayNotExpired                        0x37 //Request denied because timeout after failed security access had not expired yet
 #define UDS_NRC_uploadDownloadNotAccepted                          0x70 //Attempt to download / upload data rejected due to some issue
 #define UDS_NRC_transferDataSuspended                              0x71//Transfer of data aborted due to some issue
 #define UDS_NRC_generalProgrammingFailure                          0x72 //Erasing or programming via UDS failed
 #define UDS_NRC_wrongBlockSequenceCounter                          0x73 //Error detected in BS (Block Sequence) counter values by the ECU
 #define UDS_NRC_ResponsePending                                    0x78 //Request is valid, but ECU is not ready yet to process it
 #define UDS_NRC_subFunctionNotSupportedInActiveSession             0x7E //Current session does not have permission to perform the request
 #define UDS_NRC_rpmTooHigh                                         0x81 //current RPM is too high to perform request
 #define UDS_NRC_rpmTooLow                                          0x82 //current RPM is too low to perform request
 #define UDS_NRC_engineIsRunning                                    0x83 //Request cannot be performed with engine running
 #define UDS_NRC_engineIsNotRunning                                 0x84 //Request cannot be performed with engine not running
 #define UDS_NRC_engineRunTimeTooLow                                0x85 //Request cannot be performed because the engine has not been running for long enough
 #define UDS_NRC_temperatureTooHigh                                 0x86 //Temperature is too high to perform request
 #define UDS_NRC_temperatureTooLow                                  0x87 //Temperature is too low to perform request
 #define UDS_NRC_vehicleSpeedTooHigh                                0x88 //Vehicle speed is too high to perform request
 #define UDS_NRC_vehicleSpeedTooLow                                 0x89 //Vehicle speed is too low to perform request
 #define UDS_NRC_PedalTooHigh                                       0x8A //Throttle or pedal is in a position too high to perform request
 #define UDS_NRC_PedalTooLow                                        0x8B//Throttle or pedal is in a position too low to perform request
 #define UDS_NRC_transmissionRangeNotInNeutral                      0x8C //Transmission is not in neutral
 #define UDS_NRC_transmissionRangeNotInGear                         0x8D//Transmission is not in gear
 #define UDS_NRC_brakeSwitchNotClosed                               0x8F //Brakes are not pressed
 #define UDS_NRC_shifterLeverNotInPark                              0x90 //Shifter is not in park mode
 #define UDS_NRC_torqueConverterClutchLocked                        0x91 //TCC (Torque Converter Clutch) is in a locked state
 #define UDS_NRC_voltageTooHigh                                     0x92 //Specific voltage in the ECU is too high to perform the request
 #define UDS_NRC_voltageTooLow                                      0x93 //Specific voltage in the ECU is too low to perform the request
 
 #define UDS_WRITE_DATA_BY_IDENTIFIER                               0x2E
 #define UDS_READ_DATA_BY_IDENTIFIER                                0x22
 #define UDS_ROUTING                                                0x44
 #define UDS_Session_Control                                        0x10
 #define UDS_Session_Control_Default_Session                        0x01
 #define UDS_Session_Control_Programming_Session                    0x02 
 #define UDS_Session_Control_Extended_Session                       0x03
 #define UDS_Session_Control_Development_Session                    0x40
 #define UDS_ECU_Reset                                              0x11
 #define UDS_ECU_Reset_HardReset                                    0x01 
 #define UDS_ECU_Reset_KeyOnOffReset                                0x02
 #define UDS_ECU_Reset_SoftReset                                    0x03
 
 
 #define HEXCODE_BIT1_READ_IDENT 0xF1
 
 
#endif /*  NRC_UDS_PROTOCOL_COMMON_H */