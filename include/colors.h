/* --- ANSI Color Codes --- */
/* Added by Komo, 251022    */

void do_colortest(struct char_data *ch, char *argument, int cmd);

#define C_NRM  "\x1b[0m"  /* 리셋(Normal) */

/* 일반 색상 (글자) */
#define C_BLK  "\x1b[30m" /* Black */
#define C_RED  "\x1b[31m" /* Red */
#define C_GRN  "\x1b[32m" /* Green */
#define C_YEL  "\x1b[33m" /* Yellow */
#define C_BLU  "\x1b[34m" /* Blue */
#define C_MAG  "\x1b[35m" /* Magenta */
#define C_CYN  "\x1b[36m" /* Cyan */
#define C_WHT  "\x1b[37m" /* White */

/* 굵고 밝은 색상 (글자) */
#define C_B_BLK  "\x1b[1;30m" /* Bright Black (Gray) */
#define C_B_RED  "\x1b[1;31m" /* Bright Red */
#define C_B_GRN  "\x1b[1;32m" /* Bright Green */
#define C_B_YEL  "\x1b[1;33m" /* Bright Yellow */
#define C_B_BLU  "\x1b[1;34m" /* Bright Blue */
#define C_B_MAG  "\x1b[1;35m" /* Bright Magenta */
#define C_B_CYN  "\x1b[1;36m" /* Bright Cyan */
#define C_B_WHT  "\x1b[1;37m" /* Bright White */

/* --- [신규] 256색상 확장 매크로 (48개) --- */

/* 회색조 (Grays) */
#define C_GRY1_N "\x1b[38;5;238m"
#define C_GRY1_B "\x1b[38;5;244m"
#define C_GRY2_N "\x1b[38;5;248m"
#define C_GRY2_B "\x1b[38;5;252m"
#define C_GRY3_N "\x1b[38;5;252m" /* 6번과 중복이지만 밝기 조절용 */
#define C_GRY3_B "\x1b[38;5;255m"

/* 빨강/분홍 (Reds/Pinks) */
#define C_RED1_N "\x1b[38;5;88m"
#define C_RED1_B "\x1b[38;5;196m"
#define C_RED2_N "\x1b[38;5;124m"
#define C_RED2_B "\x1b[38;5;203m"
#define C_PNK1_N "\x1b[38;5;205m"
#define C_PNK1_B "\x1b[38;5;219m"
#define C_PNK2_N "\x1b[38;5;126m"
#define C_PNK2_B "\x1b[38;5;175m"

/* 갈색/주황 (Browns/Oranges) */
#define C_BRN1_N "\x1b[38;5;94m"
#define C_BRN1_B "\x1b[38;5;208m"
#define C_ORA1_N "\x1b[38;5;166m"
#define C_ORA1_B "\x1b[38;5;214m"
#define C_GLD1_N "\x1b[38;5;172m"
#define C_GLD1_B "\x1b[38;5;220m"
#define C_TAN1_N "\x1b[38;5;179m"
#define C_TAN1_B "\x1b[38;5;227m"

/* 녹색/노랑 (Greens/Yellows) */
#define C_OLV1_N "\x1b[38;5;58m"
#define C_OLV1_B "\x1b[38;5;154m"
#define C_FOR1_N "\x1b[38;5;22m"
#define C_FOR1_B "\x1b[38;5;118m"
#define C_GRN1_N "\x1b[38;5;28m"
#define C_GRN1_B "\x1b[38;5;83m"
#define C_TEA1_N "\x1b[38;5;30m"
#define C_TEA1_B "\x1b[38;5;85m"
#define C_MNT1_N "\x1b[38;5;78m"
#define C_MNT1_B "\x1b[38;5;157m"
#define C_GRN2_N "\x1b[38;5;29m"
#define C_GRN2_B "\x1b[38;5;49m"

/* 파랑/보라 (Blues/Purples) */
#define C_NVY1_N "\x1b[38;5;17m"
#define C_NVY1_B "\x1b[38;5;21m"
#define C_BLU1_N "\x1b[38;5;18m"
#define C_BLU1_B "\x1b[38;5;63m"
#define C_IND1_N "\x1b[38;5;55m"
#define C_IND1_B "\x1b[38;5;117m"
#define C_BLU2_N "\x1b[38;5;25m"
#define C_BLU2_B "\x1b[38;5;69m"
#define C_PUR1_N "\x1b[38;5;53m"
#define C_PUR1_B "\x1b[38;5;93m"
#define C_VIO1_N "\x1b[38;5;91m"
#define C_VIO1_B "\x1b[38;5;129m"
#define C_MAG1_N "\x1b[38;5;89m"
#define C_MAG1_B "\x1b[38;5;141m"

/* (선택적) 배경색 */
#define C_BG_BLK "\x1b[40m"
#define C_BG_RED "\x1b[41m"
#define C_BG_GRN "\x1b[42m"
#define C_BG_YEL "\x1b[43m"
#define C_BG_BLU "\x1b[44m"
#define C_BG_MAG "\x1b[45m"
#define C_BG_CYN "\x1b[46m"
#define C_BG_WHT "\x1b[47m"