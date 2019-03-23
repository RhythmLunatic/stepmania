#ifndef PYTHON23IO_H
#define PYTHON23IO_H

#include "ProductInfo.h" // Used to look for PRODUCT_ID_BARE which means STEPMANIA 5, NOT OITG
#include <stdint.h>
#include "io/USBDriver.h"
#include "arch/ACIO/ACIO.h"
#include "PrefsManager.h"
#include "arch/COM/serial.h"


#define DDR_PAD_UP 0xFD
#define DDR_PAD_DOWN 0xFB
#define DDR_PAD_LEFT 0xF7
#define DDR_PAD_RIGHT 0xEF
#define DDR_CP_LEFT 0xBF
#define DDR_CP_SELECT 0xFE
#define DDR_CP_RIGHT 0x7F
#define DDR_CP_UP_P1 0xFE //byte 4
#define DDR_CP_UP_P2 0xFB //byte 4
#define DDR_CP_DOWN_P1 0xFD // byte 4
#define DDR_CP_DOWN_P2 0xF7 // byte 4
#define DDR_TEST 0xBF //byte 4?
#define DDR_SERVICE 0xEF //byte 4?
#define DDR_COIN	0xDF

#define BIT(i) (1<<(i))
#define BIT_IS_SET(v,i) ((v&BIT(i))!=0)
#define BYTE_BIT_M_TO_L(i) (1<<(7-i))
#define BYTE_BIT_IS_SET_M_TO_L(v,i) ((v&BIT(7-i))!=0)


#define HDXB_SET_LIGHTS 0x12

#ifdef PRODUCT_ID_BARE
#ifdef STDSTRING_H
#define PSTRING RString
#else
#define PSTRING std::string
#endif
#else
#define PSTRING CString
#endif

class Python23IO : public USBDriver
{
public:
	static bool DeviceMatches(int iVendorID, int iProductID);
	bool Open();

	bool interruptRead(uint8_t* data);
	bool writeLights(uint8_t* payload);
	bool writeLightsP2IO(uint8_t* payload);
	bool openHDXB();
	bool openVEXTIO();
	bool sendUnknownCommand();
	bool getVersion();
	bool initHDXB2();
	bool initHDXB3();
	bool initHDXB4();
	bool pingHDXB();
	bool spamBaudCheck();
	bool writeHDXB(uint8_t* payload, int len, uint8_t opcode = HDXB_SET_LIGHTS);
	bool writeVEXTIO(uint8_t* payload, int len);
	bool readHDXB(int len = 0x7e);
	bool readVEXTIO(int len = 0x40);
	
	void HDXBAllOnTest();
	bool nodeCount();
	bool hasVEXTIO();

	void Reconnect();
	void FlushBulkReadBuffer();


	/* Globally accessible for diagnostics purposes. */
	static int m_iInputErrorCount;
	static PSTRING m_sInputError;


private:
	int GetResponseFromBulk(uint8_t* response, int response_length, bool output_to_log = false, bool force_override = false);
	bool WriteToBulkWithExpectedReply(uint8_t* message, bool init_packet, bool output_to_log = false);
	uint8_t checkInput(uint8_t x, uint8_t y);
	void InitHDAndWatchDog();

	static uint8_t acio_request[256];
	static uint8_t acio_response[256];
	static uint8_t python23io_request[256];
	static uint8_t python23io_response[256];
	static uint8_t pchunk[3][4];
	char debug_message[2048];
	static serial::Serial com4;
	bool baud_pass;//  = false;
	bool hdxb_ready;// = false;
	bool board_isP2IO;// = false;
	bool board_hasHDXB;// = false;
	bool board_hasVEXTIO;// = false;
	bool m_bConnected = false;
	bool COM4SET=false;
	static int bulk_reply_size;// = 0;
	static uint8_t packet_sequence;
	static const int REQ_TIMEOUT = 1000;
	static uint8_t hdxb_vcom_port;// = 0x00;
	static uint8_t hxdb_vbaud_rate;// = 0x03; //this is 2 on p2io
	static uint8_t extio_vcom_port;// = 0x00;//p2io only
	static uint8_t extio_vbaud_rate;// = 0x03;//p2io only
	static uint8_t hdxb_dev_id; // if user connects ONLY HDXB this they can change this but really they shouldn't
	static const int interrupt_ep = 0x83;
	static const int bulk_write_to_ep = 0x02;
	static const int bulk_read_from_ep = 0x81;
	static const uint16_t python3io_VENDOR_ID[2];// = { 0x0000, 0x1CCF };
	static const uint16_t python3io_PRODUCT_ID[2];// = { 0x5731, 0x8008 };
	static const uint16_t python2io_VENDOR_ID[1];// = { 0x0000 };
	static const uint16_t python2io_PRODUCT_ID[1];// = { 0x7305 };
	static Preference<PSTRING> m_COM4PORT;
	static Preference<PSTRING> m_sP23IOMode;
	static Preference<bool> m_bP2IOEXTIO;
	static Preference<int> m_iP2IO_HDXB_DEV_ID;


};

#endif /* PYTHON23IO_H */

