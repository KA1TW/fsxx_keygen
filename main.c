#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>


/******************************************************************************/
/*** Macros                                                                 ***/
/******************************************************************************/
/* Option flags */
#define FLAGS__INVALID		(0 << 0)
#define FLAGS__VALID		(1 << 0)


/******************************************************************************/
/*** Global data                                                            ***/
/******************************************************************************/
/* Options list */
static const struct option_t {
	unsigned long  seed;          /* Option seed key */
	unsigned int   flags;         /* Option flags */
	char           *description;  /* Option description string */
} options[] = {
	{ 0x069AF1C1, FLAGS__VALID,   "K5  - GSM/EDGE Application Firmware" },
	{ 0x0632E6E4, FLAGS__VALID,   "K7  - AM/FM/PM Measurement Demodulator" },
	{ 0x2AAC5519, FLAGS__VALID,   "B17 - IQ Online" },
	{ 0x081B95E1, FLAGS__INVALID, "None" },
	{ 0x2E03E151, FLAGS__VALID,   "K84 - 1xEV-DO BTS Application Firmware" },
	{ 0x19EF1119, FLAGS__VALID,   "K84 - 1xEV-DO MS  Application Firmware" },
	{ 0x310EE121, FLAGS__INVALID, "None" },
	{ 0x1756CE90, FLAGS__INVALID, "FSP-B15 FSP-B70 FS-K7" },
	{ 0x1919A691, FLAGS__INVALID, "K72 prior K74" },
	{ 0x03DAD7E4, FLAGS__INVALID, "None" },
	{ 0x2196F099, FLAGS__VALID,   "K9  - Power Meter" },
	{ 0x3DD9B339, FLAGS__INVALID, "FSP-B15 FS-K7" },
	{ 0x0E395E40, FLAGS__INVALID, "None" },
	{ 0x195B9344, FLAGS__INVALID, "None" },
	{ 0x2C7CE440, FLAGS__VALID,   "K76 - 3GPP TD-SCDMA BTS Application Firmware" },
	{ 0x08B0D139, FLAGS__VALID,   "K77 - 3GPP TD-SCDMA MS  Application Firmware" },
	{ 0x13DE9644, FLAGS__VALID,   "K30 - Noise Figure Measurament" },
	{ 0x0CFD9839, FLAGS__VALID,   "K82 - CDMA2000 BTS Application Firmware" },
	{ 0x1AF16EA9, FLAGS__VALID,   "K83 - CDMA2000 MS  Application Firmware" },
	{ 0x11E04FA4, FLAGS__VALID,   "K8  - Bluetooth Application Firmware" },
	{ 0x0BF1BE90, FLAGS__VALID,   "K40 - Phase Noise Measurament" },
	{ 0x03685351, FLAGS__INVALID, "None" },
	{ 0x1A368DB9, FLAGS__INVALID, "None" },
	{ 0x3A8A28B9, FLAGS__INVALID, "None" },
	{ 0x03BA3B10, FLAGS__INVALID, "None" },
	{ 0x0AF61A31, FLAGS__INVALID, "31 days trial period" },
	{ 0x053D6400, FLAGS__INVALID, "78 days trial period" },
	{ 0x0D659100, FLAGS__INVALID, "None" },
	{ 0x3F26B9A4, FLAGS__INVALID, "None" },
	{ 0x2626FF90, FLAGS__INVALID, "Frequency Extension" },
	{ 0x1E0D3564, FLAGS__INVALID, "None" },
	{ 0x26E9C161, FLAGS__INVALID, "Trasducer Set" },
	{ 0x2EFDA244, FLAGS__INVALID, "None" },
	{ 0x16BF9900, FLAGS__INVALID, "None" },
	{ 0x0EAFB3A1, FLAGS__INVALID, "None" },
	{ 0x36C143D9, FLAGS__INVALID, "None" },
	{ 0x0F64FEB1, FLAGS__INVALID, "None" },
	{ 0x1CC87A91, FLAGS__INVALID, "None" },
	{ 0x3C3E8900, FLAGS__INVALID, "None" },
	{ 0x3936AC24, FLAGS__INVALID, "None" },
	{ 0x2C810F89, FLAGS__INVALID, "None" },
	{ 0x2C2CFE40, FLAGS__INVALID, "None" },
	{ 0x08BA6899, FLAGS__INVALID, "None" },
	{ 0x20230E90, FLAGS__INVALID, "None" },
	{ 0x157EA044, FLAGS__INVALID, "None" },
	{ 0x39D8CC61, FLAGS__INVALID, "None" },
	{ 0x19C84B04, FLAGS__INVALID, "None" },
	{ 0x13B57040, FLAGS__INVALID, "None" },
	{ 0x30D449C4, FLAGS__INVALID, "None" },
	{ 0x20418E41, FLAGS__INVALID, "None" },
	{ 0, 0, 0 }
};

//
// Valori per Addizione/Sottrazione da opzione
//
static const unsigned long table[] = {
	0x07D00D940, 0x08488D256, 0x11FBDB21, 0x08838C862,
	0x0CF78A3D3, 0x07CBCB35C, 0x5F7C7140, 0x0CCBD55D8,
	0x01115AE60, 0x0E940852B, 0x651F66D0, 0x0BF9CF792,
	0x0DEFCD8D5, 0x0082D0E96, 0x0CD7B345, 0x0A64FD784
};

/* Decryption permutation table */
static const unsigned char decrypt_table[] = {
	0x09, 0x0A, 0x04, 0x1F, 0x1D, 0x15, 0x06, 0x17, 0x1E, 0x1B, 0x1C, 0x0E, 0x10, 0x02, 0x07, 0x0D,
	0x03, 0x05, 0x16, 0x08, 0x0C, 0x19, 0x18, 0x00, 0x1A, 0x01, 0x14, 0x0B, 0x12, 0x13, 0x11, 0x0F,
	0x1F, 0x0C, 0x1B, 0x11, 0x0D, 0x09, 0x0A, 0x15, 0x08, 0x04, 0x07, 0x1A, 0x17, 0x06, 0x01, 0x13,
	0x03, 0x0B, 0x0E, 0x05, 0x0F, 0x00, 0x16, 0x19, 0x12, 0x14, 0x1C, 0x02, 0x10, 0x1D, 0x1E, 0x18,
	0x0E, 0x02, 0x0D, 0x06, 0x1C, 0x12, 0x18, 0x0B, 0x0F, 0x1A, 0x16, 0x04, 0x1E, 0x05, 0x00, 0x1D,
	0x03, 0x13, 0x11, 0x01, 0x08, 0x07, 0x0C, 0x1F, 0x10, 0x17, 0x19, 0x15, 0x14, 0x1B, 0x09, 0x0A,
	0x0D, 0x0F, 0x1E, 0x19, 0x0E, 0x10, 0x0B, 0x13, 0x02, 0x14, 0x17, 0x01, 0x12, 0x05, 0x00, 0x09,
	0x03, 0x11, 0x0A, 0x06, 0x08, 0x15, 0x1C, 0x16, 0x1B, 0x1F, 0x1A, 0x18, 0x1D, 0x0C, 0x04, 0x07,
	0x10, 0x19, 0x08, 0x02, 0x05, 0x11, 0x16, 0x06, 0x07, 0x03, 0x0D, 0x14, 0x1C, 0x12, 0x1A, 0x18,
	0x1E, 0x13, 0x1F, 0x1D, 0x0A, 0x01, 0x0E, 0x00, 0x0C, 0x09, 0x15, 0x1B, 0x17, 0x0B, 0x0F, 0x04,
	0x18, 0x1C, 0x13, 0x09, 0x0F, 0x01, 0x05, 0x11, 0x00, 0x03, 0x17, 0x1A, 0x06, 0x1D, 0x10, 0x1F,
	0x0D, 0x12, 0x02, 0x16, 0x19, 0x0C, 0x08, 0x04, 0x0B, 0x15, 0x0A, 0x07, 0x1E, 0x1B, 0x14, 0x0E,
	0x10, 0x0A, 0x09, 0x04, 0x14, 0x08, 0x1F, 0x00, 0x1D, 0x05, 0x1B, 0x02, 0x0B, 0x12, 0x15, 0x03,
	0x0E, 0x13, 0x19, 0x16, 0x1E, 0x11, 0x0C, 0x17, 0x0D, 0x0F, 0x1A, 0x01, 0x1C, 0x07, 0x06, 0x18,
	0x05, 0x14, 0x1B, 0x0E, 0x06, 0x03, 0x07, 0x0A, 0x04, 0x11, 0x00, 0x01, 0x1F, 0x09, 0x18, 0x16,
	0x1A, 0x15, 0x0F, 0x1D, 0x08, 0x17, 0x02, 0x0D, 0x12, 0x10, 0x1C, 0x1E, 0x0B, 0x0C, 0x13, 0x19,
	0x06, 0x1E, 0x17, 0x07, 0x0A, 0x0E, 0x1D, 0x08, 0x0B, 0x14, 0x12, 0x01, 0x15, 0x0C, 0x09, 0x1F,
	0x19, 0x00, 0x0D, 0x16, 0x02, 0x04, 0x13, 0x0F, 0x18, 0x1B, 0x1C, 0x11, 0x1A, 0x05, 0x10, 0x03,
	0x16, 0x1B, 0x1F, 0x10, 0x12, 0x1A, 0x07, 0x0E, 0x0C, 0x02, 0x08, 0x1C, 0x13, 0x14, 0x05, 0x17,
	0x19, 0x03, 0x1D, 0x1E, 0x15, 0x0D, 0x0B, 0x01, 0x00, 0x06, 0x18, 0x0A, 0x0F, 0x11, 0x04, 0x09,
	0x0F, 0x17, 0x07, 0x1B, 0x18, 0x05, 0x1D, 0x0C, 0x1E, 0x08, 0x19, 0x0A, 0x11, 0x00, 0x1C, 0x0E,
	0x02, 0x13, 0x16, 0x1F, 0x1A, 0x04, 0x0D, 0x06, 0x15, 0x12, 0x01, 0x10, 0x14, 0x03, 0x09, 0x0B,
	0x08, 0x0A, 0x05, 0x1F, 0x1C, 0x09, 0x15, 0x19, 0x1E, 0x03, 0x13, 0x1A, 0x04, 0x14, 0x17, 0x01,
	0x16, 0x06, 0x00, 0x0C, 0x10, 0x0D, 0x02, 0x0E, 0x1D, 0x07, 0x0F, 0x0B, 0x11, 0x18, 0x1B, 0x12,
	0x17, 0x0A, 0x0B, 0x1C, 0x1B, 0x00, 0x19, 0x0E, 0x03, 0x11, 0x18, 0x0C, 0x07, 0x02, 0x05, 0x04,
	0x1A, 0x01, 0x15, 0x12, 0x10, 0x08, 0x16, 0x06, 0x1E, 0x1F, 0x0D, 0x14, 0x1D, 0x09, 0x0F, 0x13,
	0x0A, 0x15, 0x1F, 0x1B, 0x06, 0x07, 0x13, 0x1D, 0x02, 0x0D, 0x17, 0x1C, 0x12, 0x00, 0x1E, 0x16,
	0x1A, 0x01, 0x05, 0x0C, 0x09, 0x03, 0x08, 0x10, 0x19, 0x0F, 0x11, 0x18, 0x0E, 0x0B, 0x04, 0x14,
	0x05, 0x12, 0x0E, 0x03, 0x01, 0x02, 0x0B, 0x18, 0x17, 0x0A, 0x15, 0x13, 0x0D, 0x16, 0x1D, 0x07,
	0x14, 0x1C, 0x1A, 0x10, 0x00, 0x0C, 0x06, 0x1B, 0x11, 0x0F, 0x04, 0x1E, 0x08, 0x1F, 0x19, 0x09,
	0x1E, 0x1C, 0x05, 0x0E, 0x14, 0x18, 0x06, 0x15, 0x0D, 0x13, 0x0C, 0x10, 0x08, 0x1F, 0x19, 0x12,
	0x04, 0x0B, 0x1D, 0x16, 0x03, 0x07, 0x00, 0x1A, 0x09, 0x1B, 0x11, 0x02, 0x0A, 0x0F, 0x01, 0x17
};


/* Encryption permutation table */
static const unsigned char encrypt_table[] = {
	0x17, 0x19, 0x0D, 0x10, 0x02, 0x11, 0x06, 0x0E, 0x13, 0x00, 0x01, 0x1B, 0x14, 0x0F, 0x0B, 0x1F,
	0x0C, 0x1E, 0x1C, 0x1D, 0x1A, 0x05, 0x12, 0x07, 0x16, 0x15, 0x18, 0x09, 0x0A, 0x04, 0x08, 0x03,
	0x15, 0x0E, 0x1B, 0x10, 0x09, 0x13, 0x0D, 0x0A, 0x08, 0x05, 0x06, 0x11, 0x01, 0x04, 0x12, 0x14,
	0x1C, 0x03, 0x18, 0x0F, 0x19, 0x07, 0x16, 0x0C, 0x1F, 0x17, 0x0B, 0x02, 0x1A, 0x1D, 0x1E, 0x00,
	0x0E, 0x13, 0x01, 0x10, 0x0B, 0x0D, 0x03, 0x15, 0x14, 0x1E, 0x1F, 0x07, 0x16, 0x02, 0x00, 0x08,
	0x18, 0x12, 0x05, 0x11, 0x1C, 0x1B, 0x0A, 0x19, 0x06, 0x1A, 0x09, 0x1D, 0x04, 0x0F, 0x0C, 0x17,
	0x0E, 0x0B, 0x08, 0x10, 0x1E, 0x0D, 0x13, 0x1F, 0x14, 0x0F, 0x12, 0x06, 0x1D, 0x00, 0x04, 0x01,
	0x05, 0x11, 0x0C, 0x07, 0x09, 0x15, 0x17, 0x0A, 0x1B, 0x03, 0x1A, 0x18, 0x16, 0x1C, 0x02, 0x19,
	0x17, 0x15, 0x03, 0x09, 0x1F, 0x04, 0x07, 0x08, 0x02, 0x19, 0x14, 0x1D, 0x18, 0x0A, 0x16, 0x1E,
	0x00, 0x05, 0x0D, 0x11, 0x0B, 0x1A, 0x06, 0x1C, 0x0F, 0x01, 0x0E, 0x1B, 0x0C, 0x13, 0x10, 0x12,
	0x08, 0x05, 0x12, 0x09, 0x17, 0x06, 0x0C, 0x1B, 0x16, 0x03, 0x1A, 0x18, 0x15, 0x10, 0x1F, 0x04,
	0x0E, 0x07, 0x11, 0x02, 0x1E, 0x19, 0x13, 0x0A, 0x00, 0x14, 0x0B, 0x1D, 0x01, 0x0D, 0x1C, 0x0F,
	0x07, 0x1B, 0x0B, 0x0F, 0x03, 0x09, 0x1E, 0x1D, 0x05, 0x02, 0x01, 0x0C, 0x16, 0x18, 0x10, 0x19,
	0x00, 0x15, 0x0D, 0x11, 0x04, 0x0E, 0x13, 0x17, 0x1F, 0x12, 0x1A, 0x0A, 0x1C, 0x08, 0x14, 0x06,
	0x0A, 0x0B, 0x16, 0x05, 0x08, 0x00, 0x04, 0x06, 0x14, 0x0D, 0x07, 0x1C, 0x1D, 0x17, 0x03, 0x12,
	0x19, 0x09, 0x18, 0x1E, 0x01, 0x11, 0x0F, 0x15, 0x0E, 0x1F, 0x10, 0x02, 0x1A, 0x13, 0x1B, 0x0C,
	0x11, 0x0B, 0x14, 0x1F, 0x15, 0x1D, 0x00, 0x03, 0x07, 0x0E, 0x04, 0x08, 0x0D, 0x12, 0x05, 0x17,
	0x1E, 0x1B, 0x0A, 0x16, 0x09, 0x0C, 0x13, 0x02, 0x18, 0x10, 0x1C, 0x19, 0x1A, 0x06, 0x01, 0x0F,
	0x18, 0x17, 0x09, 0x11, 0x1E, 0x0E, 0x19, 0x06, 0x0A, 0x1F, 0x1B, 0x16, 0x08, 0x15, 0x07, 0x1C,
	0x03, 0x1D, 0x04, 0x0C, 0x0D, 0x14, 0x00, 0x0F, 0x1A, 0x10, 0x05, 0x01, 0x0B, 0x12, 0x13, 0x02,
	0x0D, 0x1A, 0x10, 0x1D, 0x15, 0x05, 0x17, 0x02, 0x09, 0x1E, 0x0B, 0x1F, 0x07, 0x16, 0x0F, 0x00,
	0x1B, 0x0C, 0x19, 0x11, 0x1C, 0x18, 0x12, 0x01, 0x04, 0x0A, 0x14, 0x03, 0x0E, 0x06, 0x08, 0x13,
	0x12, 0x0F, 0x16, 0x09, 0x0C, 0x02, 0x11, 0x19, 0x00, 0x05, 0x01, 0x1B, 0x13, 0x15, 0x17, 0x1A,
	0x14, 0x1C, 0x1F, 0x0A, 0x0D, 0x06, 0x10, 0x0E, 0x1D, 0x07, 0x0B, 0x1E, 0x04, 0x18, 0x08, 0x03,
	0x05, 0x11, 0x0D, 0x08, 0x0F, 0x0E, 0x17, 0x0C, 0x15, 0x1D, 0x01, 0x02, 0x0B, 0x1A, 0x07, 0x1E,
	0x14, 0x09, 0x13, 0x1F, 0x1B, 0x12, 0x16, 0x00, 0x0A, 0x06, 0x10, 0x04, 0x03, 0x1C, 0x18, 0x19,
	0x0D, 0x11, 0x08, 0x15, 0x1E, 0x12, 0x04, 0x05, 0x16, 0x14, 0x00, 0x1D, 0x13, 0x09, 0x1C, 0x19,
	0x17, 0x1A, 0x0C, 0x06, 0x1F, 0x01, 0x0F, 0x0A, 0x1B, 0x18, 0x10, 0x03, 0x0B, 0x07, 0x0E, 0x02,
	0x14, 0x04, 0x05, 0x03, 0x1A, 0x00, 0x16, 0x0F, 0x1C, 0x1F, 0x09, 0x06, 0x15, 0x0C, 0x02, 0x19,
	0x13, 0x18, 0x01, 0x0B, 0x10, 0x0A, 0x0D, 0x08, 0x07, 0x1E, 0x12, 0x17, 0x11, 0x0E, 0x1B, 0x1D,
	0x16, 0x1E, 0x1B, 0x14, 0x10, 0x02, 0x06, 0x15, 0x0C, 0x18, 0x1C, 0x11, 0x0A, 0x08, 0x03, 0x1D,
	0x0B, 0x1A, 0x0F, 0x09, 0x04, 0x07, 0x13, 0x1F, 0x05, 0x0E, 0x17, 0x19, 0x01, 0x12, 0x00, 0x0D
} ;


/******************************************************************************/
/*** Static functions                                                       ***/
/******************************************************************************/
/* Encrypt/Decrypt 32-bit function */
static unsigned long chiper(unsigned long nibble, unsigned long value, unsigned char encrypt)
{
	unsigned char  bit;
	unsigned long  chiper = 0;

	/* Iterate all 32 bits */
	for (bit = 0; bit < 32; bit++)
		if (value & (1 << bit))
			chiper |= 1 << (encrypt ? encrypt_table[(nibble << 5) + bit] :
			                          decrypt_table[(nibble << 5) + bit]);

	return chiper;
}


#ifdef DEBUG
//
// Funzione di Decrypt Key Code
//
static unsigned long decrypt(const char *keyascii, unsigned long serialnr)
{
	unsigned char x;
	unsigned long shift;
	unsigned long key;
	unsigned long keytemp;

	// Converto Key in unsigned long
	key = strtoul(keyascii, NULL, 10);

	// Inizializzo variabili
	shift   = serialnr;		// Seriale Strumento
	keytemp = key;			// Seriale immesso per opzione

	// Inizializzo contatori loop
	for(x=0; x<8; x++)
	{
		keytemp = chiper(shift & 0x0F, keytemp - table[shift & 0x0F], 0);

		shift = shift >> 4;
	}

	// Esco con valore opzione
	return keytemp;
}
#endif /* DEBUG */


//
// Funzione di Encrypt Opzione
//
static unsigned long encrypt(unsigned long opzione, unsigned long serialnr)
{
	unsigned char x;
	unsigned long shift;

	// Inizializzo contatori loop
	for(x=0; x<8; x++)
	{
		shift = serialnr >> (4 * (7-x));

		opzione = chiper(shift & 0x0F, opzione, 1) + table[shift & 0x0F];
	}

	// Esco con valore opzione
	return opzione;
}


static int get_serialnr(const char *optarg, unsigned long *serialnr)
{
	char  *endptr;

	/* Retrieve the number before the slash */
	*serialnr = 1000 * strtoul(optarg, &endptr, 10);

	/* Bail with an error if no slash follows */
	if (*endptr != '/')
		goto err_serialnr;

	/* Bail with an error if nothing follows the slash */
	endptr++;
	if (*endptr == '\0')
		goto err_serialnr;

	/* Add the number following the slash */
	*serialnr += strtoul(endptr, &endptr, 10);

	/* Bail with an error if anything follows */
	if (*endptr != '\0')
		goto err_serialnr;

	return 0;

err_serialnr:
	fprintf(stderr, "Invalid serial number '%s'\n", optarg);
	fprintf(stderr, "Serial numbers are formatted like 123456/789\n");

	return -1;
}


/*****************************************************************************/
/*** Functions                                                             ***/
/*****************************************************************************/
int main(int argc, char* argv[])
{
	int            result = EXIT_SUCCESS;
	int            arg;
	unsigned long  serialnr = 0;

	/* Process the command line arguments */
	while ((arg = getopt(argc, argv, "hs:")) != -1) {
		switch (arg) {
		case 's':
			if (get_serialnr(optarg, &serialnr))
				return EXIT_FAILURE;
			break;

		default:
			result = EXIT_FAILURE;
		case 'h':
			fprintf(stderr, "Usage:\n");
			fprintf(stderr, "%s -s serialnr\n", basename(argv[0]));
			return result;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc) {
		fprintf(stderr, "Stdin not supported\n");
		result = EXIT_FAILURE;
		goto err_arg;
	}

#ifdef DEBUG
	fprintf(stderr, "decrypt(\"0123456789\", %ld) = 0x%.8lx\n", serialnr, decrypt("0123456789", serialnr));
	fprintf(stderr, "encrypt(0x%.8lx,   %ld) = 0x%.8lx\n", options->seed, serialnr, encrypt(options->seed, serialnr));
#endif /* DEBUG */

	if (serialnr) {
		unsigned long          key;
		const struct option_t  *option = options;

		// Per tutte le opzioni dispinibili
		while (option->seed)
		{
			// Controllo se opzione valida
			if (option->flags & FLAGS__VALID)
			{
				// Calcolo chiave
				key = encrypt(option->seed, serialnr);

				// Stampo valore opzione da immettere nello strumento
				printf("%010lu - %s\r\n", key, option->description);
			}

			// Opzione successiva
			option++;
		}
	}

err_arg:
	return result;
}
