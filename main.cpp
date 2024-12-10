#include<bits/stdc++.h>
using namespace std;

// Loading the ratings from the CSV file
vector<vector<int>> loadRatings(const string& filename) {
    vector<vector<int>> ratings;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        vector<int> row;
        size_t pos = 0;
        while ((pos = line.find(',')) != string::npos) {
            row.push_back(stoi(line.substr(0, pos)));
            line.erase(0, pos + 1);
        }
        row.push_back(stoi(line)); 
        ratings.push_back(row);
    }

    file.close();
    return ratings;
}

// Calculating the cosine similarity between two users
double calculateSimilarity(const vector<int>& user1, const vector<int>& user2) {
    double dotProduct = 0.0, norm1 = 0.0, norm2 = 0.0;
    for (size_t i = 0; i < user1.size(); i++) {
        dotProduct += user1[i] * user2[i];
        norm1 += user1[i] * user1[i];
        norm2 += user2[i] * user2[i];
    }
    if (norm1 == 0 || norm2 == 0) return 0; 
    return dotProduct / (sqrt(norm1) * sqrt(norm2));
}

// Predicting ratings for a specific user
vector<double> predictRatings(const vector<vector<int>>& ratings, int targetUser) {
    size_t numMovies = ratings[0].size();
    vector<double> predictedRatings(numMovies, 0.0);
    vector<double> similarityScores(ratings.size(), 0.0);

    // Calculating similarities with other users
    for (size_t i = 0; i < ratings.size(); i++) {
        if (i != targetUser) {
            similarityScores[i] = calculateSimilarity(ratings[targetUser], ratings[i]);
        }
    }

    // Predicting ratings using weighted average
    for (size_t movie = 0; movie < numMovies; movie++) {
        double weightedSum = 0.0, similaritySum = 0.0;
        for (size_t user = 0; user < ratings.size(); user++) {
            if (user != targetUser && ratings[user][movie] > 0) {
                weightedSum += similarityScores[user] * ratings[user][movie];
                similaritySum += abs(similarityScores[user]);
            }
        }
        if (similaritySum > 0) {
            predictedRatings[movie] = weightedSum / similaritySum;
        }
    }

    return predictedRatings;
}

// Recommending top N movies
vector<int> recommendMovies(const vector<double>& predictedRatings, const vector<int>& originalRatings, int N) {
    vector<pair<double, int>> movieScores;

    for (size_t i = 0; i < predictedRatings.size(); i++) {
        if (originalRatings[i] == 0) { 
            movieScores.push_back({predictedRatings[i], static_cast<int>(i)});
        }
    }

    sort(movieScores.rbegin(), movieScores.rend());

    // Selecting top N movies
    vector<int> recommendations;
    for (int i = 0; i < N && i < movieScores.size(); i++) {
        recommendations.push_back(movieScores[i].second);
    }

    return recommendations;
}

int main() {
    // Movie names used statically
    vector<string> movieNames = {
        "Inception", "The Dark Knight", "Interstellar", "The Matrix", "Parasite"
    };

    // Loading ratings from CSV file
    string filename = "ratings.csv"; 
    vector<vector<int>> ratings = loadRatings(filename);

    int targetUser; 
    int topN = 3; // Max Number of recommendations

    // Asking the user to input the target user (1-based index)
    cout << "Enter the user number (1 to " << ratings.size() << ") for recommendations: ";
    cin >> targetUser;

    // Validating the user input dynamically
    if (targetUser < 1 || targetUser > ratings.size()) {
        cout << "Invalid user number! Please run the program again and provide a valid user number." << endl;
        return 1;
    }

    // Converting to 0-based index
    targetUser -= 1;

    // Predicting ratings for the target user
    vector<double> predictedRatings = predictRatings(ratings, targetUser);

    // Recommending top N movies
    vector<int> recommendations = recommendMovies(predictedRatings, ratings[targetUser], topN);

    // Displaying recommendations with movie names
    cout << "Recommended movies for User " << targetUser + 1 << ": ";
    for (int movie : recommendations) {
        cout << movieNames[movie] << " ";
    }
    cout << endl;

    return 0;
}
