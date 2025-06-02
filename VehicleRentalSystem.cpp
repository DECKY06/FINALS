#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <limits>

using namespace std;


class BillingStrategy {
public:
    virtual double calculateCharge(int days) = 0;
    virtual ~BillingStrategy() {}
};

class DailyRate : public BillingStrategy {
public:
    double calculateCharge(int days) override {
        return days * 50.0; // $50 per day
    }
};

class WeekendRate : public BillingStrategy {
public:
    double calculateCharge(int days) override {
        return days * 40.0; // $40 per day
    }
};


class User {
protected:
    string username, password;
public:
    User(string u, string p) : username(u), password(p) {}
    virtual void menu() = 0;
    bool login(string u, string p) {
        return (username == u && password == p);
    }
};


class Vehicle {
public:
    int id;
    string model;
    bool available;

    Vehicle(int id, string model) : id(id), model(model), available(true) {}
};


class Booking {
public:
    int bookingID;
    int vehicleID;
    int days;
    double charge;

    Booking(int id, int vid, int d, double c) : bookingID(id), vehicleID(vid), days(d), charge(c) {}
};


class Customer : public User {
    vector<Vehicle>& vehicles;
    vector<Booking>& bookings;
    unique_ptr<BillingStrategy> billing;

public:
    Customer(string u, string p, vector<Vehicle>& v, vector<Booking>& b)
        : User(u, p), vehicles(v), bookings(b) {}

    void menu() override {
        int choice;
        do {
            cout << "\nCustomer Menu:\n1. View Vehicles\n2. Book Vehicle\n3. Return Vehicle\n4. Exit\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: viewVehicles(); break;
                case 2: bookVehicle(); break;
                case 3: returnVehicle(); break;
                case 4: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 4);
    }

    void viewVehicles() {
        for (auto& v : vehicles) {
            cout << "ID: " << v.id << " | Model: " << v.model
                 << " | Available: " << (v.available ? "Yes" : "No") << endl;
        }
    }

    void bookVehicle() {
        int id, days;
        cout << "Enter Vehicle ID to book: ";
        cin >> id;
        for (auto& v : vehicles) {
            if (v.id == id && v.available) {
                cout << "Enter number of days: ";
                cin >> days;
                if (days <= 0) {
                    cout << "Invalid day count.\n";
                    return;
                }

                cout << "Select Billing Type: 1. Daily 2. Weekend: ";
                int type;
                cin >> type;

                if (type == 1) billing = make_unique<DailyRate>();
                else if (type == 2) billing = make_unique<WeekendRate>();
                else {
                    cout << "Invalid billing type.\n";
                    return;
                }

                double charge = billing->calculateCharge(days);
                bookings.emplace_back(bookings.size() + 1, id, days, charge);
                v.available = false;
                cout << "Booking successful! Charge: $" << charge << endl;
                return;
            }
        }
        cout << "Vehicle not available or ID invalid.\n";
    }

    void returnVehicle() {
        int id;
        cout << "Enter Vehicle ID to return: ";
        cin >> id;
        for (auto& v : vehicles) {
            if (v.id == id && !v.available) {
                v.available = true;
                cout << "Vehicle returned successfully.\n";
                return;
            }
        }
        cout << "Invalid return.\n";
    }
};


class Admin : public User {
    vector<Vehicle>& vehicles;

public:
    Admin(string u, string p, vector<Vehicle>& v) : User(u, p), vehicles(v) {}

    void menu() override {
        int choice;
        do {
            cout << "\nAdmin Menu:\n1. Add Vehicle\n2. View Vehicles\n3. Exit\nChoice: ";
            cin >> choice;
            switch (choice) {
                case 1: addVehicle(); break;
                case 2: viewVehicles(); break;
                case 3: cout << "Logging out...\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 3);
    }

    void addVehicle() {
        int id;
        string model;
        cout << "Enter Vehicle ID: ";
        cin >> id;
        cout << "Enter Vehicle Model: ";
        cin.ignore();
        getline(cin, model);
        vehicles.emplace_back(id, model);
        cout << "Vehicle added.\n";
    }

    void viewVehicles() {
        for (auto& v : vehicles) {
            cout << "ID: " << v.id << " | Model: " << v.model
                 << " | Available: " << (v.available ? "Yes" : "No") << endl;
        }
    }
};


int main() {
    vector<Vehicle> vehicles;
    vector<Booking> bookings;

   
    vehicles.push_back(Vehicle(101, "Toyota Vios"));
    vehicles.push_back(Vehicle(102, "Honda Civic"));
    vehicles.push_back(Vehicle(103, "Suzuki Ertiga"));

    Admin admin("admin", "admin123", vehicles);
    Customer customer("user", "pass", vehicles, bookings);

    string uname, pwd;
    int choice;

    while (true) {
        cout << "\n--- Vehicle Rental System ---\n";
        cout << "1. Login\n2. Exit\nChoice: ";
        cin >> choice;

        if (choice == 2) break;

        cout << "Username: ";
        cin >> uname;
        cout << "Password: ";
        cin >> pwd;

        try {
            if (admin.login(uname, pwd)) {
                admin.menu();
            } else if (customer.login(uname, pwd)) {
                customer.menu();
            } else {
                throw runtime_error("Login failed: Invalid credentials.");
            }
        } catch (exception& e) {
            cerr << "Error: " << e.what() << endl;
        }
    }

    return 0;
}
