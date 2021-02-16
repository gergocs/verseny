//g++ main.cpp -l sqlite3

#include <iostream>
#include <sqlite3.h>
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>

#define database "rezdb.db"

#define PORT 8080

#define closer "GET /close"
#define json "GET /json"
#define inserter "GET /insert/"
#define deleter "GET /delete/"

using namespace std;

string gdata;

struct datas{
    string name;
    string clas;
    string place;
    int type{};
    int current{};
    bool cprotect{};
    bool good{};
    int resistance{};
    string final;
};

static int callback(void* data, int argc, char** argv, char** azColName){

    gdata += "\n\t\"reports\": [\n \t\t{\n";

    for(int i = 0; i < argc; i++){
        gdata += "\t\t\t\"";
        gdata += azColName[i];
        gdata += "\": \"";
        gdata += argv[i] ? argv[i] : "NULL";
        if(i != argc-1){
            gdata += "\",\n";
        }else{
            gdata += "\"\n";
        }

    }
    gdata += "\t\t}\n \t]";
    return 0;
}

int main() {
    sqlite3* db;

    struct sockaddr_in address{};

    char* messaggeError;

    int server_fd, new_socket;
    int opt = 1;
    int addrlen = sizeof(address);
    int exit;

    char buffer[2048] = {0};

    string success = "done";
    string error = "error";
    string closed = "closed";
    string tmp;

    size_t found;

    const string query = "SELECT * FROM reports";
    const string last = "SELECT id FROM reports ORDER BY id DESC LIMIT 1";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        return EXIT_FAILURE;
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        return EXIT_FAILURE;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("bind failed");
        return EXIT_FAILURE;
    }if(listen(server_fd, 3) < 0){
        perror("listen");
        return EXIT_FAILURE;
    }

    exit = sqlite3_open(database, &db);

    sqlite3_exec(db, query.c_str(), callback, nullptr, nullptr);

    if(exit != SQLITE_OK){
        cerr << "Error " << sqlite3_errmsg(db) << endl;
        cout<<exit<<endl;
        return -1;
    }else{
        cout<<"DB opened"<<endl;
    }

    do{
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        read( new_socket , buffer, 1024);

        tmp = buffer;

        found = tmp.find(closer);

        if(found != string::npos) {
            send(new_socket , closed.c_str(), closed.length() , 0 );
            close(new_socket);
            break;
        }

        found = tmp.find(json);

        if(found != string::npos){
            gdata = "{";

            sqlite3_exec(db, query.c_str(), callback, nullptr, nullptr);
            gdata += "\n}\n";
            send(new_socket , gdata.c_str(), gdata.length() , 0 );
        }

        found = tmp.find(deleter);

        if(found != string::npos){
            string str;

            size_t found2 = tmp.find("HTTP");

            for(unsigned int i = found+12; i < found2-1; i++){
                str+=tmp[i];
            }
            string sql = "DELETE FROM reports WHERE id = " + str + ";";

            exit = sqlite3_exec(db, sql.c_str(), nullptr, 0, &messaggeError);

            if(exit != SQLITE_OK){
                cerr << " ERROR DELETE" << endl;
                sqlite3_free(messaggeError);
                send(new_socket , error.c_str(), error.length(), 0 );
            }else{
                cout<<"deleted "<<str<<endl;
                send(new_socket , success.c_str(), success.length(), 0 );
            }

        }

        found = tmp.find(inserter);

        if(found != string::npos){
            size_t found2 = tmp.find("HTTP");
            size_t found3;
            unsigned int counter = 0;
            datas tmp2;
            string str;
            for(unsigned int i = found+12; i < found2-1; i++){
                if(tmp[i]=='/'){
                    if(counter == 3){
                        tmp2.type = stoi(str);
                    }else if(counter == 4){
                        tmp2.current = stoi(str);
                    }else if(counter == 5){
                        tmp2.cprotect = stoi(str);
                    }else if(counter == 6){
                        tmp2.good = stoi(str);
                    }else if(counter == 7){
                        tmp2.resistance = stoi(str);
                    }
                    str = "";
                    counter++;
                    continue;
                }
                if(counter == 0){
                    tmp2.name += tmp[i];
                }else if(counter == 1){
                    tmp2.clas += tmp[i];
                }else if(counter == 2){
                    tmp2.place += tmp[i];
                }else if(counter == 3){
                    str += tmp[i];
                }else if(counter == 4){
                    str += tmp[i];
                }else if(counter == 5){
                    str += tmp[i];
                }else if(counter == 6){
                    str += tmp[i];
                }else if(counter == 7){
                    str += tmp[i];
                }else if(counter == 8){
                    tmp2.final += tmp[i];
                }
            }

            gdata = "{\n";

            sqlite3_exec(db, last.c_str(), callback, nullptr, nullptr);

            found3 = gdata.find("\"id\": \"");
            str = "";

            if(found3 != string::npos){
                size_t found4 = gdata.find("\"\n");
                for(unsigned int i = found3+7; i < found4; i++){
                    str += gdata[i];
                }
                counter = stoi(str);
                counter++;
            }

            string insert = "INSERT INTO reports VALUES(";
                    insert += to_string(counter);
                    insert += ",\"";
                    insert += tmp2.name + "\",\"" + tmp2.clas + "\", \"" + tmp2.place + "\", ";
                    insert += to_string(tmp2.type);
                    insert += ",";
                    insert += to_string(tmp2.current);
                    insert += ",";
                    insert += to_string(tmp2.cprotect);
                    insert += ",";
                    insert += to_string(tmp2.good);
                    insert += ",";
                    insert += to_string(tmp2.resistance);
                    insert += ",\"";
                    insert += tmp2.final;
                    insert += "\");";
            cout << insert << endl;
            exit = sqlite3_exec(db, insert.c_str(), NULL, 0, &messaggeError);
            if(exit != SQLITE_OK){
                cerr << "Error Insert" << endl;
                sqlite3_free(messaggeError);
                send(new_socket , error.c_str(), error.length(), 0 );
            }else{
                cout << "Done" << endl;
                send(new_socket , success.c_str(), success.length(), 0 );
            }

        }
        close(new_socket);
    }while(true);

    sqlite3_close(db);

    return 0;
}