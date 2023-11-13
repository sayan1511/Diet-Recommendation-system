#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include<string.h>
#include <iomanip>
#include<cmath>
#include <algorithm>
#include <map>
#include <numeric>
#include <set>
using namespace std;

struct UserData{
    int age;
    double weight;
    int height;
    double bmi;
    string profession;
    string isVegetarian;
    int kcal;
    string gender;
};

void read_data(string gender,string pre, int min_age, int max_age,string profession, vector<string>& matchage) {
    fstream fin;
    int age;
    fin.open("class.csv", ios::in);

    vector<string> row;
    string line, word, temp;
    bool skipFirstRow = true;

    while (getline(fin, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        if (row.size() > 2) {
            if (row[10] == gender) {
                if(row[6] == pre){
                    if(row[8] == profession){
                        try {
                            age = stoi(row[2]);
                            if (age >= min_age && age <= max_age) {
                                matchage.push_back(row[0]);
                            }
                        } catch (const invalid_argument& e) {
                            cerr << "Error: Cannot convert age to an integer in row: " << line << endl;
                        }
                    }
                }    
            }
        }
    }
    fin.close();
}

void read_BMI(double bmi,vector<string>& matchbmi){
    fstream fin;
    fin.open("class.csv",ios::in);

    vector<string> row;
    string line, word, temp;
    bool skipFirstRow = true;

    while (getline(fin, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        if (row.size() > 5) {
            try {
                double bmi2 = stod(row[5]);
                //cout<< bmi2;
                if (bmi2 == bmi) {
                    matchbmi.push_back(row[0]);
                }
            } catch (const invalid_argument& e) {
                cerr << "Error: Cannot convert age to an integer in row: " << line << endl;
            }
        }
    }
    fin.close(); 
}


//calculate BMI func
double BMI(double weight,double height){
    //store the height in Meter
    double h1=height/100.0;
    double bmi= weight/(h1*h1);
    return bmi;

}


void calculate_avg_ratings(const vector<string>& pids) {
    fstream fin;
    fin.open("ratings.csv", ios::in);

    vector<string> row;
    string line, word, temp;
    bool skipFirstRow = true;

    map<string, vector<pair<string, double>> > ratingsMap;

    while (getline(fin, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        if (row.size() >= 3) {
            if (find(pids.begin(), pids.end(), row[0]) != pids.end()) {
                const string& pid = row[0];
                const string& fid = row[1];
                double rating = stod(row[2]);
                ratingsMap[pid].push_back(make_pair(fid, rating));
            }
        }
    }
    fin.close();

    for (const auto& entry : ratingsMap) {
        const string& pid = entry.first;
        const vector<pair<string, double>>& ratings = entry.second;

        cout << "PID: " << pid << endl;
        double sum = 0.0;

        for (const auto& fidRating : ratings) {
            const string& fid = fidRating.first;
            const double rating = fidRating.second;

            cout << "FID: " << fid << ", Rating: " << rating << endl;
            sum += rating;
        }

        double avgRating = ratings.empty() ? 0.0 : sum / ratings.size();
        cout << "Average Rating: " << avgRating << endl;
        cout << "------------------------------" << endl;
    }
}


void calculate_avg_rating_for_fid(const string& targetFID) {
    fstream ratingsFile;
    ratingsFile.open("ratings.csv", ios::in);

    //map to store ratings for the given FID
    map<string, vector<double>> ratingsMap;

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

    //ratings data and populate the ratingsMap
    while (getline(ratingsFile, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        if (row.size() >= 3) {
            string fid = row[1];
            double rating = stod(row[2]);

            //FID matches the target FID
            if (fid == targetFID) {
                ratingsMap[targetFID].push_back(rating);
            }
        }
    }

    ratingsFile.close();

    // Calculate the average rating for the target FID
    double avgRating = 0.0;
    if (!ratingsMap[targetFID].empty()) {
        vector<double>& ratings = ratingsMap[targetFID];

        // Calculate the average rating
        double sum = accumulate(ratings.begin(), ratings.end(), 0.0);
        avgRating = sum / ratings.size();
    }

    // Print the average rating and find the FID's name from food.csv
    if (avgRating > 0) {
        fstream foodFile;
        foodFile.open("food.csv", ios::in);

        bool found = false;
        while (getline(foodFile, line)) {
            row.clear();
            stringstream s(line);

            while (getline(s, word, ',')) {
                row.push_back(word);
            }

            if (row.size() >= 2 && row[0] == targetFID) {
                cout << "FID: " << targetFID << ", Name: " << row[2] << ", Average Rating: " << avgRating << endl;
                found = true;
                break;
            }
        }

        foodFile.close();

        if (!found) {
            cout << "FID: " << targetFID << ", Name: Not Found, Average Rating: " << avgRating << endl;
        }
    } else {
        cout << "FID: " << targetFID << ", No ratings found" << endl;
    }
}


void calculate_similarity(int age, double weight, int height, double bmi, string isveg, string profe, string gender, const vector<string>& matchingPIDs) {
    fstream fin;
    fin.open("class.csv", ios::in);

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

    double dage = 0.3;
    double dweight = 0.2;
    double dheight = 0.2;
    double dbmi = 0.1;
    double disveg = 0.05;
    double dprofe = 0.05;
    double dgender = 0.1;

    double totalWeight = dage + dweight + dheight + dbmi + disveg + dprofe + dgender;

    map<string, double> similarityScores;  // Store PID -> Similarity Score

    while (getline(fin, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        // Check if the PID is in the matchingPIDs array
        if (row.size() >= 8 && find(matchingPIDs.begin(), matchingPIDs.end(), row[0]) != matchingPIDs.end()) {
            int dbAge = stoi(row[2]);
            double dbWeight = stod(row[3]);
            int dbHeight = stoi(row[4]);
            double dbBmi = stod(row[5]);
            string dbIsVeg = row[6];
            string dbProfe = row[8];
            string dbGender = row[10];

            // Calculate the weighted similarity for each attribute
            double ageSimilarity = dage * (1.0 - abs(age - dbAge) / static_cast<double>(age));
            double weightSimilarity = dweight * (1.0 - abs(weight - dbWeight) / weight);
            double heightSimilarity = dheight * (1.0 - abs(height - dbHeight) / static_cast<double>(height));
            double bmiSimilarity = dbmi * (1.0 - abs(bmi - dbBmi) / bmi);
            double isvegSimilarity = (isveg == dbIsVeg) ? disveg : 0.0;
            double profeSimilarity = (profe == dbProfe) ? dprofe : 0.0;
            double genderSimilarity = (gender == dbGender) ? dgender : 0.0;

            // Calculate the total similarity for this row
            double rowSimilarity = (ageSimilarity + weightSimilarity + heightSimilarity + bmiSimilarity +
                                    isvegSimilarity + profeSimilarity + genderSimilarity) / totalWeight;

            // Store the similarity score in the map with the PID as the key
            similarityScores[row[0]] = rowSimilarity;
        }
    }
    fin.close();

    // Print similarity scores for the matching PIDs
    cout << "Matching PIDs and their Similarity Scores:\n";
    for (const auto& pair : similarityScores) {
        cout << "PID: " << pair.first << ", Similarity Score: " << pair.second << endl;
    }
}


void recordFoodRatings(map<string, double>& foodRatings) {
    map<string, string> foodData; // FID -> Food Name
    fstream foodFile;
    foodFile.open("food.csv", ios::in);

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

    while (getline(foodFile, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        if (row.size() >= 2) {
            foodData[row[0]] = row[2]; // Store FID and Food Name
        }
    }
    foodFile.close();

    string ratefood;

    cout << "Do you want to rate foods? (yes or no): ";
    cin >> ratefood;

    if (ratefood == "no" || ratefood == "n") {
        cout << "Exiting the food rating process." << endl;
        return;
    }
    if (ratefood == "yes" || ratefood == "y"){
        cout << "FID - Food Name" << endl;
            for (const auto& food : foodData) {
                cout << food.first << " - " << food.second << endl;
        }
    }

    while (ratefood != "exit") {
        cout << "\nChoose a food item to rate:" << endl;

        string fid;
        double rating;

        cout << "Enter the FID of the food item (or 'exit' to exit): ";
        cin >> fid;

        if (fid == "exit") {
            cout << "Exiting the food rating process." << endl;
            break; // Exit loop
        }

        //enter the FID valid
        if (foodData.find(fid) != foodData.end()) {
            cout << "Rate the food item (out of 5): ";
            cin >> rating;

            if (rating >= 1.0 && rating <= 5.0) {
                // rating store
                foodRatings[fid] = rating;
                cout << "Rating recorded for " << foodData.at(fid) << endl;
            } else {
                cout << "Invalid rating. Please enter a rating between 1 and 5." << endl;
            }
        } else {
            cout << "Invalid FID. Please enter a valid FID." << endl;
        }
    }
}


double calculateAverageUserRating(const map<string, double>& foodRatings) {
    if (foodRatings.empty()) {
        return 0.0;  // No ratings given
    }

    double totalRating = 0.0;
    int numberOfRatings = 0;

    for (const auto& pair : foodRatings) {
        totalRating += pair.second;  // Sum all the ratings
        numberOfRatings++;
    }

    double averageRating = totalRating / numberOfRatings;

    return averageRating;
}

double calculateAvgRatingForPID(const string& targetPID) {
    fstream ratingsFile;
    ratingsFile.open("ratings.csv", ios::in);

    // Map to store ratings for the given PID
    map<string, vector<double>> ratingsMap;

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

    // Read ratings data and populate the ratingsMap
    while (getline(ratingsFile, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        if (row.size() >= 3) {
            string pid = row[0];
            double rating = stod(row[2]);

            // PID matches the target PID
            if (pid == targetPID) {
                ratingsMap[targetPID].push_back(rating);
            }
        }
    }

    ratingsFile.close();

    // Calculate the average rating for the target PID
    double avgRating = 0.0;
    if (!ratingsMap[targetPID].empty()) {
        vector<double>& ratings = ratingsMap[targetPID];

        // Calculate the average rating
        double sum = accumulate(ratings.begin(), ratings.end(), 0.0);
        avgRating = sum / ratings.size();
    }

    return avgRating;
}



void findMatchingFIDs(const vector<string>& matchingPIDs, const map<string, double>& userRatedFIDs, double useravgrating, double pidavgrating) {
    map<string, set<string>> pidToRatedFIDs;

    fstream ratingsFile;
    ratingsFile.open("ratings.csv", ios::in);

    vector<string> row;
    string line, word;
    bool skipFirstRow = true;

    // Read the ratings data and populate the pidToRatedFIDs map
    while (getline(ratingsFile, line)) {
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        if (skipFirstRow) {
            skipFirstRow = false;
            continue;
        }

        if (row.size() >= 3) {
            string pid = row[0];
            string fid = row[1];
            double rate = stod(row[2]);

            // Check if the PID is one of the matching PIDs
            if (find(matchingPIDs.begin(), matchingPIDs.end(), pid) != matchingPIDs.end()) {
                pidToRatedFIDs[pid].insert(fid);
            }
        }
    }

    ratingsFile.close();

    vector<pair<string, double>> matchingFIDsAndRatings;

    // Find FIDs rated by both the user and matching PIDs
    for (const auto& pair : pidToRatedFIDs) {
        const set<string>& pidRatedFIDs = pair.second;

        for (const string& fid : pidRatedFIDs) {
            if (userRatedFIDs.find(fid) != userRatedFIDs.end()) {
                // Fetch the user's rating for this FID
                double userRating = userRatedFIDs.at(fid);
                double pidRating = 0.0;  // You need to fetch the PID's rating for this FID from your data

                // Calculate the correlation for this FID
                double userAvgRating = useravgrating;
                double pidAvgRating = pidavgrating;

                double userDiff = userRating - userAvgRating;
                double pidDiff = pidRating - pidAvgRating;

                double numerator = userDiff * pidDiff;
                double userSquaredDiff = userDiff * userDiff;
                double pidSquaredDiff = pidDiff * pidDiff;

                double denominator = sqrt(userSquaredDiff * pidSquaredDiff);

                // Avoid division by zero by checking the denominator
                if (denominator != 0.0) {
                    double correlationValue = numerator / denominator;
                    cout << "FID: " << fid << ", User Rating: " << userRating << ", Correlation: " << correlationValue << endl;
                } else {
                    cout << "FID: " << fid << ", User Rating: " << userRating << ", Correlation: undefined (division by zero)" << endl;
                }
            }
        }
    }
}
int main(){
    UserData userInput;

//take user data 
    cout << "Enter your Gender: ";
    cin >> userInput.gender;
    
    cout << "Enter your age: ";
    cin >> userInput.age;

    cout <<"Enter your Height: ";
    cin >> userInput.height;

    cout << "Enter your weight: ";
    cin >> userInput.weight;

    cout << "Enter your profession: ";
    cin.ignore();  // Clear newline from previous input
    getline(cin, userInput.profession);

    cout << "Enter your preference (veg/non-veg): ";
    cin >>  userInput.isVegetarian;

    // cout << "Do you want to gain weight of lose(gain/lose)";
    // cin >> userInput.isgain;

    userInput.bmi=BMI(userInput.weight,userInput.height);
    userInput.bmi = round(userInput.bmi * 100) / 100;
    cout << userInput.bmi << "\n"; 

//declearing vec str for matching age profe BMI
    vector<string>matchage;
    vector<string>matchbmi;
    vector<string>matchingpidlist;

    cout << "Recommonded calories intake of age in between " ;
//data match for gender,preferance,age,profession and print match values in given range
    if (userInput.age >= 16 && userInput.age <= 22) {
        if(userInput.gender == "Male"){
            cout << "16-22 years: About 2,200 - 2,800 calories per day";
        }
        else {
            cout << "16-22 years: About 1,800 - 2,200 calories per day" ;
        }
        read_data(userInput.gender,userInput.isVegetarian,16, 22,userInput.profession, matchage);
    } else if (userInput.age >= 23 && userInput.age <= 28) {
                if(userInput.gender == "Male"){
            cout << "23-28 years:About 2,200 - 2,800 calories per day";
        }
        else {
            cout << "23-28 years: About 1,800 - 2,200 calories per day" ;
        }
        read_data(userInput.gender,userInput.isVegetarian,23, 28,userInput.profession, matchage);
    } else if (userInput.age >= 29 && userInput.age <= 35) {
        if(userInput.gender == "Male"){
            cout << "29-35 years: About 2,200 - 2,800 calories per day";
        }
        else {
            cout << "29-35 years:About 1,800 - 2,200 calories per day" ;
        }
        read_data(userInput.gender,userInput.isVegetarian,29,35,userInput.profession, matchage);
    } else if (userInput.age >= 36 && userInput.age <= 43) {
        if(userInput.gender == "Male"){
            cout << "36-43 years: About 2,200 - 2,800 calories per day";
        }
        else {
            cout << "36-43 years:About 1,800 - 2,200 calories per day" ;
        }
        read_data(userInput.gender,userInput.isVegetarian,36,43,userInput.profession, matchage);
    } else if (userInput.age >= 44 && userInput.age <= 65) {
        if(userInput.gender == "Male"){
            cout << "44-65 years: About 2,000 - 2,600 calories per day";
        }
        else {
            cout << "44-65 years:About 1,600 - 2,200 calories per day" ;
        }
        read_data(userInput.gender,userInput.isVegetarian,44,65,userInput.profession, matchage);
    } else {
        cout << "Age is not in any category.\n";
    }
    if (!matchage.empty()) {
        cout << "\nMatching PIDs:\n";
        for (const string& pid : matchage) {
            cout << pid << endl;
            matchingpidlist.push_back(pid);
        }
    } else {
        cout << "\nNo data matched\n";
    }

//call bmi and print match value
cout << "Want to check who have similar BMI with you ?(yes/no || y/n)";
string mbmi;
cin >> mbmi;
if(mbmi == "yes" || mbmi == "y"){
    read_BMI(userInput.bmi,matchbmi);
        if (!matchbmi.empty()) {
            cout << "Matching PIDs(bmi):\n";
            for (const string& pid : matchbmi) {
                cout << pid << endl;
            }
        } else {
            cout << "No data matched(bmi)\n";
        }
    }


    calculate_similarity(userInput.age,userInput.weight,userInput.height,userInput.bmi,userInput.isVegetarian,userInput.profession,userInput.gender,matchingpidlist);
//find and print ratings for matching pid
     calculate_avg_ratings(matchingpidlist);

//check rating of a particular food and print name and avg rating
    string check_rating;
    string f;
    while (true) {
        cout << "Do you want to see what others have given rating to any particular food item? (y/n): ";
        cin >> check_rating;

        if (check_rating == "n") {
            break;
        } else if (check_rating == "y") {
            cout << "Please provide the FID: ";
            cin >> f;
            calculate_avg_rating_for_fid(f);
        } else {
            cout << "Invalid input. Please enter 'y' for yes or 'n' for no." << endl;
        }
    }

    map<string, double> foodRatings;

    recordFoodRatings(foodRatings);

    double userAverageRating = calculateAverageUserRating(foodRatings);
    cout << "Your average Rating: " << userAverageRating << endl;

    string targetPID;
    cout << "Enter the PID with whom you want to find correlation: ";
    cin >> targetPID;
    double avgRating = calculateAvgRatingForPID(targetPID);
    cout << "Average Rating for " << targetPID << ": " << avgRating << endl;

    findMatchingFIDs(matchingpidlist, foodRatings, userAverageRating, avgRating);

    return 0;    

}

//