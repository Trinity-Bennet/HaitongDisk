#include "protocol.h"


PDU *mkPDU(uint ui_msg_len)
{
    uint ui_pdu_len = sizeof(PDU) + ui_msg_len;
    PDU * pdu = (PDU *)malloc(ui_pdu_len);
    memset(pdu,0,ui_pdu_len);
    if(pdu == NULL){
        exit(EXIT_FAILURE);
    }
    memset(pdu,0,ui_pdu_len);
    pdu->ui_pdu_len = ui_pdu_len;
    pdu->ui_msg_len = ui_msg_len;
    return pdu;
}
