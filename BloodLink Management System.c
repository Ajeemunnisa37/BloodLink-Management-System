#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DONOR_FILE "donors.dat"
#define RECEIVER_FILE "receivers.dat"
#define MAX_NAME 50
#define MAX_LOC 30
#define MAX_BG 5
#define MAX_GENDER 10
#define MAX_CONTACT 20
typedef struct {
    int day;
    int month;
    int year;
} Date;
typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
    char gender[MAX_GENDER];
    float weight;
    char bloodGroup[MAX_BG];
    char location[MAX_LOC];
    char contact[MAX_CONTACT];
    Date lastDonation;
    int donationCount;
    int loyaltyScore;
    int eligible;
} Donor;
typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
    char gender[MAX_GENDER];
    char neededGroup[MAX_BG];
    char location[MAX_LOC];
    char urgency[10];
    char contact[MAX_CONTACT];
} Receiver;
void clearInput();
void readLine(char *buf, int size);
char toLowerManual(char ch);
int strContainsIgnoreCase(const char *str, const char *substr);
int getNextDonorID();
int getNextReceiverID();
int dateToDays(Date d);
int daysBetween(Date last, Date today);
int isEligible(int age, float weight, Date lastDonation, Date today);
void addDonor();
void viewDonors();
void searchDonor();
void updateDonor();
void updateReceiver();
void deleteDonor();
void addReceiver();
void viewReceivers();
int isCompatible(const char donorGroup[], const char recvGroup[]);
void matchDonorReceiver();
void mainMenu();
void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void readLine(char *buf, int size) {
    if (fgets(buf, size, stdin) != NULL) {
        size_t ln = strlen(buf);
        if (ln > 0 && buf[ln - 1] == '\n') buf[ln - 1] = '\0';
    } else buf[0] = '\0';
}
char toLowerManual(char ch) {
    if (ch >= 'A' && ch <= 'Z')
        return ch + 32;
    else
        return ch;
}
char toUpperManual(char ch) {
    if (ch >= 'a' && ch <= 'z')
        return ch - 32;
    else
        return ch;
}

int strContainsIgnoreCase(const char *str, const char *substr) {
    char a[256], b[256];
    int i;
    strncpy(a, str, 255); a[255] = '\0';
    strncpy(b, substr, 255); b[255] = '\0';

    for (i = 0; a[i]; i++) a[i] = toLowerManual(a[i]);
    for (i = 0; b[i]; i++) b[i] = toLowerManual(b[i]);

    return strstr(a, b) != NULL;
}
int getNextDonorID() {
    FILE *fp = fopen(DONOR_FILE, "rb");
    if (!fp) return 1;
    Donor d;
    int maxID = 0;
    while (fread(&d, sizeof(Donor), 1, fp) == 1) {
        if (d.id > maxID) maxID = d.id;
    }
    fclose(fp);
    return maxID + 1;
}

int getNextReceiverID() {
    FILE *fp = fopen(RECEIVER_FILE, "rb");
    if (!fp) return 1;
    Receiver r;
    int maxID = 0;
    while (fread(&r, sizeof(Receiver), 1, fp) == 1) {
        if (r.id > maxID) maxID = r.id;
    }
    fclose(fp);
    return maxID + 1;
}
int dateToDays(Date d) {
    return d.year * 365 + d.month * 30 + d.day;
}

int daysBetween(Date last, Date today) {
    return dateToDays(today) - dateToDays(last);
}

int isEligible(int age, float weight, Date lastDonation, Date today) {
    if (age < 18 || age > 60) return 0;
    if (weight < 50.0f) return 0;
    if (daysBetween(lastDonation, today) < 90) return 0;
    return 1;
}
void addDonor() {
    Donor d;
    Date today;
    FILE *fp;

    d.id = getNextDonorID();
    printf("\nAuto-generated Donor ID: %d\n", d.id);

    clearInput();
    printf("Enter Name: ");
    readLine(d.name, MAX_NAME);
    printf("Enter Age: ");
    scanf("%d", &d.age);
    clearInput();
    printf("Enter Gender: ");
    readLine(d.gender, MAX_GENDER);
    printf("Enter Weight (kg): ");
    scanf("%f", &d.weight);
    clearInput();
    printf("Enter Blood Group: ");
    readLine(d.bloodGroup, MAX_BG);
    for (int i = 0; d.bloodGroup[i]; i++)
    d.bloodGroup[i] = toUpperManual(d.bloodGroup[i]);
    printf("Enter Location: ");
    readLine(d.location, MAX_LOC);
    printf("Enter Contact: ");
    readLine(d.contact, MAX_CONTACT);
    printf("Enter Last Donation Date (dd mm yyyy): ");
    scanf("%d %d %d", &d.lastDonation.day, &d.lastDonation.month, &d.lastDonation.year);
    printf("Enter Today's Date (dd mm yyyy): ");
    scanf("%d %d %d", &today.day, &today.month, &today.year);
    d.eligible = isEligible(d.age, d.weight, d.lastDonation, today);
    d.donationCount = 0;
    d.loyaltyScore = 0;
    if (!d.eligible) {
        printf("  Donor not eligible.\n");
        return;
    }
    fp = fopen(DONOR_FILE, "ab");
    fwrite(&d, sizeof(Donor), 1, fp);
    fclose(fp);
    printf("Donor Added Successfully!\n");
}
void viewDonors() {
    FILE *fp = fopen(DONOR_FILE, "rb");
    if (!fp) { printf("No Donor Records Found.\n"); return; }
    Donor d;
    printf("\n%-5s %-20s %-5s %-8s %-8s %-12s %-12s\n",
           "ID", "Name", "Age", "Weight", "Group", "Location", "Contact");
    while (fread(&d, sizeof(Donor), 1, fp) == 1) {
        printf("%-5d %-20s %-5d %-8.1f %-8s %-12s %-12s\n",
               d.id, d.name, d.age, d.weight, d.bloodGroup, d.location, d.contact);
    }
    fclose(fp);
}
void searchDonor() {
    FILE *fp = fopen(DONOR_FILE, "rb");
    if (!fp) { printf("No Donors Available.\n"); return; }
    char key[20];
    clearInput();
    printf("Enter Blood Group or Name to Search: ");
    readLine(key, 20);
    Donor d; int found = 0;
    while (fread(&d, sizeof(Donor), 1, fp) == 1) {
        if (strContainsIgnoreCase(d.name, key) || strContainsIgnoreCase(d.bloodGroup, key)) {
            printf("ID:%d | %s | %s | %s | %s\n",
                   d.id, d.name, d.bloodGroup, d.location, d.contact);
            found = 1;
        }
    }
    if (!found) printf("No Matching Donor Found.\n");
    fclose(fp);
}
void updateDonor() {
    FILE *fp = fopen(DONOR_FILE, "rb");
    if (!fp) { printf("No Donors Found.\n"); return; }
    FILE *temp = fopen("temp.dat", "wb");
    int id, found = 0;
    printf("Enter Donor ID to Update: ");
    scanf("%d", &id);
    Donor d;
    while (fread(&d, sizeof(Donor), 1, fp) == 1) {
        if (d.id == id) {
            found = 1;
            clearInput();
            printf("Enter changed Location: ");
            readLine(d.location, MAX_LOC);
            printf("Enter changed Contact: ");
            readLine(d.contact, MAX_CONTACT);
            printf("Enter changed blood group");
            readLine(d.bloodGroup,MAX_BG);

        }
        fwrite(&d, sizeof(Donor), 1, temp);
    }
    fclose(fp); fclose(temp);
    remove(DONOR_FILE);
    rename("temp.dat", DONOR_FILE);
    if (found) printf(" Donor Updated.\n");
    else printf(" Donor ID Not Found.\n");
}
void deleteDonor() {
    FILE *fp = fopen(DONOR_FILE, "rb");
    if (!fp) { printf("No Donor Records.\n"); return; }
    FILE *temp = fopen("temp.dat", "wb");
    int id, found = 0;
    printf("Enter Donor ID to Delete: ");
    scanf("%d", &id);
    Donor d;
    while (fread(&d, sizeof(Donor), 1, fp) == 1) {
        if (d.id != id) fwrite(&d, sizeof(Donor), 1, temp);
        else found = 1;
    }
    fclose(fp); fclose(temp);
    remove(DONOR_FILE);
    rename("temp.dat", DONOR_FILE);
    if (found) printf(" Donor Deleted.\n");
    else printf(" Donor ID Not Found.\n");
}
void addReceiver() {
    Receiver r;
    r.id = getNextReceiverID();
    clearInput();
    printf("Enter Receiver Name: ");
    readLine(r.name, MAX_NAME);
    printf("Enter Age: ");
    scanf("%d", &r.age);
    clearInput();
    printf("Enter Gender: ");
    readLine(r.gender, MAX_GENDER);
    printf("Enter Needed Blood Group: ");
    readLine(r.neededGroup, MAX_BG);
    for (int i = 0; r.neededGroup[i]; i++)
    r.neededGroup[i] = toUpperManual(r.neededGroup[i]);
    printf("Enter Location: ");
    readLine(r.location, MAX_LOC);
    printf("Enter Urgency (Low/Medium/High): ");
    readLine(r.urgency, 10);
    printf("Enter Contact: ");
    readLine(r.contact, MAX_CONTACT);
    FILE *fp = fopen(RECEIVER_FILE, "ab");
    fwrite(&r, sizeof(Receiver), 1, fp);
    fclose(fp);
    printf(" Receiver Added Successfully!\n");
}
void viewReceivers() {
    FILE *fp = fopen(RECEIVER_FILE, "rb");
    if (!fp) { printf("No Receiver Records Found.\n"); return; }
    Receiver r;
    printf("\n%-5s %-20s %-5s %-8s %-12s %-10s\n",
           "ID", "Name", "Age", "Group", "Location", "Urgency");
    while (fread(&r, sizeof(Receiver), 1, fp) == 1) {
        printf("%-5d %-20s %-5d %-8s %-12s %-10s\n",
               r.id, r.name, r.age, r.neededGroup, r.location, r.urgency);
    }
    fclose(fp);
}
int isCompatible(const char donorGroup[], const char recvGroup[]) {
    if (strcmp(recvGroup, "AB+") == 0) return 1;
    if (strcmp(donorGroup, recvGroup) == 0) return 1;
    if (strcmp(recvGroup, "O-") == 0 && strcmp(donorGroup, "O-") == 0) return 1;
    return 0;
}
void matchDonorReceiver() {
    FILE *fd = fopen(DONOR_FILE, "rb");
    FILE *fr = fopen(RECEIVER_FILE, "rb");
    if (!fd || !fr) { printf("No Donors or Receivers Available.\n"); return; }
    Receiver r;
    printf("\nAvailable Receivers:\n");
    while (fread(&r, sizeof(Receiver), 1, fr) == 1)
        printf("ID:%d | %s | %s | %s\n", r.id, r.name, r.neededGroup, r.location);
    fclose(fr);
    int rid;
    printf("Enter Receiver ID: ");
    scanf("%d", &rid);
    fr = fopen(RECEIVER_FILE, "rb");
    Receiver selected;
    int found = 0;
    while (fread(&selected, sizeof(Receiver), 1, fr) == 1)
        if (selected.id == rid) { found = 1; break; }
    fclose(fr); 
    if (!found) { printf(" Receiver Not Found.\n"); return; }
    printf("\nCompatible Donors:\n");
    Donor d; int any = 0;
    while (fread(&d, sizeof(Donor), 1, fd) == 1)
        if (isCompatible(d.bloodGroup, selected.neededGroup) && d.eligible) {
            printf("ID:%d | %s | %s | %s\n",
                   d.id, d.name, d.bloodGroup, d.location);
            any = 1;
        }
    if (!any) printf("No Compatible Donors Found.\n");
    fclose(fd);
}
void mainMenu() {
    int ch;
    while (1) {
        printf("\nBLOOD DONATION SYSTEM\n");
        printf("1. Add Donor\n2. View Donors\n3. Search Donor\n4. Update Donor\n5. Delete Donor\n");
        printf("6. Add Receiver\n7. View Receivers\n8. Match Donor & Receiver\n9. Exit\n");
        printf("Enter Choice: ");
        scanf("%d", &ch);

        switch (ch) {
            case 1: addDonor(); break;
            case 2: viewDonors(); break;
            case 3: searchDonor(); break;
            case 4: updateDonor(); break;
            case 5: deleteDonor(); break;
            case 6: addReceiver(); break;
            case 7: viewReceivers(); break;
            case 8: matchDonorReceiver(); break;
            case 9: printf("Exiting... Thank you!\n"); return;
            default: printf("Invalid choice.\n");
        }
    }
}
int main() {
    printf("Welcome to SMART BLOOD DONATION SYSTEM\n");
    mainMenu();
    return 0;
}
