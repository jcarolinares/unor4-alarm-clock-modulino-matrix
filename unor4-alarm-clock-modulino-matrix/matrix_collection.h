// UNO R4 Matrix frames

const uint32_t heart[] = {
    0x3184a444,
    0x44042081,
    0x100a0040
};

constexpr uint32_t icon_ALARM_SET[] = {
  0x0600f01f,
  0x81f81f81,
  0xf83fc060
};

constexpr uint32_t icon_ALARM[] = {
  0x0604f29f,
  0x99f95fa1,
  0xf83fc060
};

const uint32_t alarm_animation[][4] = {
  { 0x0604f29f, 0x99f95fa1, 0xf83fc060, 200 },
  { 0x0600f01f, 0x81f81f81, 0xf83fc180, 200 }
};

constexpr uint32_t frame_MON[] = {
  0x000aeeea,
  0xaaaaaaaa,
  0xea000000
};

constexpr uint32_t frame_TUE[] = {
  0x000eae4a,
  0x84ac4a84,
  0xee000000
};

constexpr uint32_t frame_WED[] = {
  0x000aeca8,
  0xaacae8aa,
  0xec000000
};

constexpr uint32_t frame_THU[] = {
  0x000eaa4a,
  0xa4ea4aa4,
  0xae000000
};

constexpr uint32_t frame_FRI[] = {
  0x000eee8a,
  0x4cc48a48,
  0xae000000
};

constexpr uint32_t frame_SAT[] = {
  0x000eee8a,
  0x4eee2a4e,
  0xa4000000
};

constexpr uint32_t frame_SUN[] = {
  0x000eaa8a,
  0xeeaa2aae,
  0xea000000
};

// 2:45 Clock face + "H" (Hour)
constexpr uint32_t icon_HOUR[] = {
  0x38044a8a,
  0xaf2e82a4,
  0x4a380000
};

// 2:45 Clock face + "M" (Minute)
constexpr uint32_t icon_MIN[] = {
  0x38044a8a,
  0xef2a82a4,
  0x4a380000
};

// END of UNO R4 Matrix frames

// constexpr const uint32_t* matrixIcon[] = {
//   LEDMATRIX_BLUETOOTH,
//   LEDMATRIX_BOOTLOADER_ON,
//   LEDMATRIX_CHIP,
//   LEDMATRIX_CLOUD_WIFI,
//   LEDMATRIX_DANGER,
//   LEDMATRIX_EMOJI_BASIC,
//   LEDMATRIX_EMOJI_HAPPY,
//   LEDMATRIX_EMOJI_SAD,
//   LEDMATRIX_HEART_BIG,
//   LEDMATRIX_HEART_SMALL,
//   LEDMATRIX_LIKE,
//   LEDMATRIX_MUSIC_NOTE,
//   LEDMATRIX_RESISTOR,
//   LEDMATRIX_UNO
// };

// constexpr const uint32_t (*matrixAnimation[])[4] = {
//   LEDMATRIX_ANIMATION_STARTUP,
//   LEDMATRIX_ANIMATION_TETRIS_INTRO,
//   LEDMATRIX_ANIMATION_ATMEGA,
//   LEDMATRIX_ANIMATION_LED_BLINK_HORIZONTAL,
//   LEDMATRIX_ANIMATION_LED_BLINK_VERTICAL,
//   LEDMATRIX_ANIMATION_ARROWS_COMPASS,
//   LEDMATRIX_ANIMATION_AUDIO_WAVEFORM,
//   LEDMATRIX_ANIMATION_BATTERY,
//   LEDMATRIX_ANIMATION_BOUNCING_BALL,
//   LEDMATRIX_ANIMATION_BUG,
//   LEDMATRIX_ANIMATION_CHECK,
//   LEDMATRIX_ANIMATION_CLOUD,
//   LEDMATRIX_ANIMATION_DOWNLOAD,
//   LEDMATRIX_ANIMATION_DVD,
//   LEDMATRIX_ANIMATION_HEARTBEAT_LINE,
//   LEDMATRIX_ANIMATION_HEARTBEAT,
//   LEDMATRIX_ANIMATION_INFINITY_LOOP_LOADER,
//   LEDMATRIX_ANIMATION_LOAD_CLOCK,
//   LEDMATRIX_ANIMATION_LOAD,
//   LEDMATRIX_ANIMATION_LOCK,
//   LEDMATRIX_ANIMATION_NOTIFICATION,
//   LEDMATRIX_ANIMATION_OPENSOURCE,
//   LEDMATRIX_ANIMATION_SPINNING_COIN,
//   LEDMATRIX_ANIMATION_TETRIS,
//   LEDMATRIX_ANIMATION_WIFI_SEARCH,
//   LEDMATRIX_ANIMATION_HOURGLASS
// };


constexpr const char* const matrixText[] = {
  "   VIETNAM ARDUINO DAYS   ",
  "   THIS IS THE NEW MODULINO MATRIX   ",
  "   Xin chao!!!   "
};


const uint32_t number_0[5] = {
		0x3fc40,
		0x24024024,
		0x23fc000,
		66,
    100
	};
  
const uint8_t bottle_frame [] = {
  0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0
};


// Numbers 0 to 9 in 3x32-bit format (for vertical 12x8 layout)
constexpr uint32_t NUM_0[3] = { 0x00182424, 0x24242424, 0x24180000 };
constexpr uint32_t NUM_1[3] = { 0x00081828, 0x08080808, 0x083C0000 };
constexpr uint32_t NUM_2[3] = { 0x00182404, 0x04081020, 0x203C0000 };
constexpr uint32_t NUM_3[3] = { 0x00182404, 0x08180404, 0x24180000 };
constexpr uint32_t NUM_4[3] = { 0x00081828, 0x487C0808, 0x08080000 };
constexpr uint32_t NUM_5[3] = { 0x003C2020, 0x38040404, 0x24180000 };
constexpr uint32_t NUM_6[3] = { 0x00182420, 0x38242424, 0x24180000 };
constexpr uint32_t NUM_7[3] = { 0x003C0404, 0x08081010, 0x20200000 };
constexpr uint32_t NUM_8[3] = { 0x00182424, 0x18242424, 0x24180000 };
constexpr uint32_t NUM_9[3] = { 0x00182424, 0x241C0404, 0x24180000 };

// BIG Numbers 0 to 9 in 3x32-bit format (Full 12x8 vertical utilization)
constexpr uint32_t BIG_NUM_0[3] = { 0x3C666666, 0x66666666, 0x6666663C };
constexpr uint32_t BIG_NUM_1[3] = { 0x18387818, 0x18181818, 0x1818187E };
constexpr uint32_t BIG_NUM_2[3] = { 0x3C666606, 0x0C183060, 0x60607E7E };
constexpr uint32_t BIG_NUM_3[3] = { 0x3C660606, 0x3E3E0606, 0x0666663C };
constexpr uint32_t BIG_NUM_4[3] = { 0x0C1C3C6C, 0x6C6C7E7E, 0x0C0C0C0C };
constexpr uint32_t BIG_NUM_5[3] = { 0x7E7E6060, 0x7C7E0606, 0x0666663C };
constexpr uint32_t BIG_NUM_6[3] = { 0x3C666060, 0x7C7E6666, 0x6666663C };
constexpr uint32_t BIG_NUM_7[3] = { 0x7E7E0606, 0x0C0C1818, 0x30303030 };
constexpr uint32_t BIG_NUM_8[3] = { 0x3C666666, 0x3C3C6666, 0x6666663C };
constexpr uint32_t BIG_NUM_9[3] = { 0x3C666666, 0x667E3E06, 0x0606663C };

// 7-Segment Style Numbers 0 to 9 in 3x32-bit format (12x8 vertical layout)
constexpr uint32_t SEG_NUM_0[3] = { 0x7E7E6666, 0x66666666, 0x66667E7E };
constexpr uint32_t SEG_NUM_1[3] = { 0x06060606, 0x06060606, 0x06060606 };
constexpr uint32_t SEG_NUM_2[3] = { 0x7E7E0606, 0x067E7E60, 0x60607E7E };
constexpr uint32_t SEG_NUM_3[3] = { 0x7E7E0606, 0x067E7E06, 0x06067E7E };
constexpr uint32_t SEG_NUM_4[3] = { 0x66666666, 0x667E7E06, 0x06060606 };
constexpr uint32_t SEG_NUM_5[3] = { 0x7E7E6060, 0x607E7E06, 0x06067E7E };
constexpr uint32_t SEG_NUM_6[3] = { 0x7E7E6060, 0x607E7E66, 0x66667E7E };
constexpr uint32_t SEG_NUM_7[3] = { 0x7E7E0606, 0x06060606, 0x06060606 };
constexpr uint32_t SEG_NUM_8[3] = { 0x7E7E6666, 0x667E7E66, 0x66667E7E };
constexpr uint32_t SEG_NUM_9[3] = { 0x7E7E6666, 0x667E7E06, 0x06067E7E };

// 7-Segment Numbers (4px wide) WITH a colon on the right side
// 7-Segment Numbers (6px wide) WITH a narrow 1px colon on the right edge
// Format: 6 columns for the digit, 1 empty column, 1 column for the colon

constexpr uint32_t COLON_NUM_0[3] = { 0xFCFCCCCD, 0xCDCCCCCD, 0xCDCCFCFC };
constexpr uint32_t COLON_NUM_1[3] = { 0x0C0C0C0D, 0x0D0C0C0D, 0x0D0C0C0C };
constexpr uint32_t COLON_NUM_2[3] = { 0xFCFC0C0D, 0x0DFCFCC1, 0xC1C0FCFC };
constexpr uint32_t COLON_NUM_3[3] = { 0xFCFC0C0D, 0x0DFCFC0D, 0x0D0CFCFC };
constexpr uint32_t COLON_NUM_4[3] = { 0xCCCCCCCD, 0xCDFCFC0D, 0x0D0C0C0C };
constexpr uint32_t COLON_NUM_5[3] = { 0xFCFCC0C1, 0xC1FCFC0D, 0x0D0CFCFC };
constexpr uint32_t COLON_NUM_6[3] = { 0xFCFCC0C1, 0xC1FCFCCD, 0xCDCCFCFC };
constexpr uint32_t COLON_NUM_7[3] = { 0xFCFC0C0D, 0x0D0C0C0D, 0x0D0C0C0C };
constexpr uint32_t COLON_NUM_8[3] = { 0xFCFCCCCD, 0xCDFCFCCD, 0xCDCCFCFC };
constexpr uint32_t COLON_NUM_9[3] = { 0xFCFCCCCD, 0xCDFCFC0D, 0x0D0CFCFC };


// Letters
constexpr uint32_t MENU_M[3] = { 0xC3E7FFDB, 0xC3C3C3C3, 0xC3C3C3C3 };
constexpr uint32_t MENU_E[3] = { 0xFFFFC0C0, 0xC0FCFCC0, 0xC0C0FFFF };
constexpr uint32_t MENU_N[3] = { 0xC3E3E3F3, 0xF3DBDBCF, 0xCFC7C7C3 };
constexpr uint32_t MENU_U[3] = { 0xC3C3C3C3, 0xC3C3C3C3, 0xC3C3FFFF };
