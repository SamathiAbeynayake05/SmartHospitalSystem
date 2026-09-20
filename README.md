# Smart Hospital Patient & Resource Allocation System

CSC1012 – Introduction to Computer Programming individual assignment.
A menu-driven console application in C simulating patient intake, bed
allocation, emergency triage sorting, and medical billing for a hospital.

## Files
- `main.c`      — program entry point, menu loop
- `hospital.c`  — all lookup data, patient records, billing calculations,
                   bed assignment, sorting, reporting, and file persistence
- `hospital.h`  — constants, lookup table declarations, patient data
                   declarations, and function prototypes

## How to build

### Code::Blocks
1. Open `SmartHospitalSystem.cbp`.
2. Build & Run (F9).

### Command line (gcc)
    gcc -Wall -o hospital main.c hospital.c
    ./hospital

## Features
- **Requirement 1:** specialty/ward lookup tables + `bedOccupancy[4][20]` matrix
- **Requirement 2:** patient intake with full input validation (rejects
  non-numeric and out-of-range input without crashing)
- **Requirement 3:** wait time, surcharge, ward cost, gross total, age
  discount, final amount — verified against the assignment's own sample
  output (LKR 48,237.50)
- **Requirement 4:** stable bubble sort by triage priority
- **Requirement 5:** formatted bill printout matching the spec's sample format
- **Requirement 6:** performance reports — counts by urgency, revenue,
  discounts, bed occupancy %, highest-paying patient
- **Requirement 7 (bonus):** `beds_status.txt` persists bed state between
  runs; `patient_records.txt` logs every billed patient permanently
- Daily patient cap per specialty is tracked and flagged when exceeded

## Assumptions
See the full list in the project report PDF (Section 5), including:
- Patient IDs are generated from array index, not stored persistently.
- A full ward auto-registers the patient as an outpatient instead of blocking.
- Wait time uses the specialty's queue count *before* the current patient.
- `MAX_PATIENTS` is capped at 100 (parallel array size).


