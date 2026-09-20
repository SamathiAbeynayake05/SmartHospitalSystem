#ifndef HOSPITAL_H
#define HOSPITAL_H

//CONSTANTS

#define NUM_SPECIALTIES   4
#define NUM_WARDS         4
#define MAX_BEDS_PER_WARD 20
#define MAX_PATIENTS      100
#define NAME_LEN          50
#define BEDS_FILE         "beds_status.txt"
#define RECORDS_FILE      "patient_records.txt"
#define NEXT_ID_FILE      "next_id.txt"

//LOOKUP DATA Requirement 1
// Parallel arrays: index 0 -> Specialty ID 1, index 1 -> ID 2, etc.
extern const char  specialtyName[NUM_SPECIALTIES][30];
extern const double specialtyBaseFee[NUM_SPECIALTIES];
extern const int    specialtyConsultTime[NUM_SPECIALTIES];  // minutes
extern const int    specialtyDailyCap[NUM_SPECIALTIES];

extern const char  wardName[NUM_WARDS][30];
extern const double wardDailyRate[NUM_WARDS];
extern const int    wardCapacity[NUM_WARDS];

/* 2D bed occupancy matrix: [wardIndex][bedIndex] -> 0 = Available, 1 = Occupied */
extern int bedOccupancy[NUM_WARDS][MAX_BEDS_PER_WARD];

/* Running queue count per specialty (Requirement 3.1) */
extern int specialtyQueueCount[NUM_SPECIALTIES];

/* Persistent patient ID counter - survives across program runs so IDs
   never repeat, even across separate sessions (fixes duplicate PAT-1001
   issue that would otherwise occur every time patientCount resets to 0) */
extern int nextPatientId;

/*  PATIENT PARALLEL ARRAYS Requirement 2  */
extern int    patientID[MAX_PATIENTS];       /* the actual persistent ID assigned at registration */
extern char   patientName[MAX_PATIENTS][NAME_LEN];
extern int    patientAge[MAX_PATIENTS];
extern int    patientUrgency[MAX_PATIENTS];      // 1,2,3
extern int    patientSpecialtyIdx[MAX_PATIENTS]; // 0-based index into specialty arrays
extern int    patientIsAdmitted[MAX_PATIENTS];   // 1 = yes, 0 = no
extern int    patientWardIdx[MAX_PATIENTS];      // 0-based index, -1 if not admitted
extern int    patientBedNumber[MAX_PATIENTS];    // 0-based bed index assigned, -1 if none
extern int    patientDaysAdmitted[MAX_PATIENTS];

// Calculated billing fields
extern double patientBaseFee[MAX_PATIENTS];
extern double patientSurcharge[MAX_PATIENTS];
extern double patientWardCost[MAX_PATIENTS];
extern double patientGrossTotal[MAX_PATIENTS];
extern double patientDiscount[MAX_PATIENTS];
extern double patientFinalAmount[MAX_PATIENTS];
extern double patientWaitTime[MAX_PATIENTS];

extern int patientCount; /* number of patients registered so far */

// FUNCTION DECLARATIONS

/* Input validation helper (new) */
int getValidatedInt(const char *prompt, int min, int max);

/* Setup / persistence */
void initializeBeds(void);
void loadBedStatus(void);
void saveBedStatus(void);
void appendPatientRecord(int idx);
void loadNextPatientId(void);
void saveNextPatientId(void);

/* Menu / flow */
void showMainMenu(void);
void registerPatient(void);
void viewBedOccupancy(void);
void viewSortedPatients(void);
void generateReports(void);

/* Core calculations (Requirement 3) */
double calculateWaitTime(int specialtyIdx);
double calculateSurcharge(int urgencyLevel, double baseFee);
double calculateWardCost(int daysAdmitted, int wardIdx);
double calculateGrossTotal(double baseFee, double surcharge, double wardCost);
double calculateDiscount(int age, double grossTotal);
double calculateFinalAmount(double grossTotal, double discount);

// Helpers
int  assignBed(int wardIdx);
void displayBill(int idx);
void sortPatientsByPriority(int order[]);
const char* urgencyLabel(int level);

#endif
