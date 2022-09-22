#include <pqxx/pqxx>
#include <iostream>
#include <typeinfo>
#include "nlohmann/json.hpp"
#include <vector>    // vector class-template definition
#include <map>

using namespace std;
using namespace pqxx;
using json = nlohmann::json;

//template <class T>
class DatabaseEngine {

	public:
		string connection_string;
		
		DatabaseEngine(void) {}

		DatabaseEngine(string connection_string) {
			this->connection_string = connection_string;
		}

		DatabaseEngine(string db, string user, string pwd, string host, string port) {
			this->connection_string = "dbname=" + db + " user=" + user + " password=" + pwd + " host=" + host + " port=" + port;
		}

		int create(string create_query) {
			
			connection c = this->connect();
			work W(c);
			/* Execute SQL query */
			W.exec(create_query);
			W.commit();
			c.close();
			return 0;
		}

		result getOne(string get_one_query) {

			try {
				connection c = this->connect();
				string sql_q = get_one_query;
				nontransaction query(c);
				result query_result(query.exec(sql_q));
				c.close();
				return query_result;
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return {};
			}
		}

		result getAll(string get_all_query) {

			try {
				connection c = this->connect();
				string sql_q = get_all_query;
				nontransaction query(c);
				result query_result(query.exec(sql_q));
				c.close();
				return query_result;
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return {};
			}
		}

		int update(string update_query) {

			connection c = this->connect();
			work W(c);
			/* Execute SQL query */
			W.exec(update_query);
			W.commit();
			c.close();
			return 0;
		}

		int remove(string remove_query) {

			connection c = this->connect();
			work W(c);
			/* Execute SQL query */
			W.exec(remove_query);
			W.commit();
			c.close();
			return 0;
		}

		connection connect() {
			try {
				connection con(this->connection_string);
				if (!con.is_open()) {
					cout << "Database error, and can not be reached " << con.dbname() << endl;
					con.close();
					return con;
				}
				return con;
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return {};
			}
		}
};

class UserOrm: public DatabaseEngine {

	public:
		string userFirstName;
		string userLastName;
		string id;
		string userEmail;
		string connection_string;
		bool empty = false;
		bool database_error = false;
		string db_error;
		string password;

		UserOrm()
			: DatabaseEngine() {
		}

		UserOrm(string connection)
			: DatabaseEngine(connection) {

		}
		UserOrm(string userFirstName, string userLastName, string id, string userEmail, string password, string connection)
			: DatabaseEngine(connection) {
			this->userEmail = userEmail;
			this->id = id;
			this->userFirstName = userFirstName;
			this->userLastName = userLastName;
		}

		UserOrm(string userFirstName, string userLastName, string userEmail, string password, string connection)
			: DatabaseEngine(connection) {
			this->userEmail = userEmail;
			this->userFirstName = userFirstName;
			this->userLastName = userLastName;
		}

		UserOrm(string userFirstName, string userLastName, string userEmail, string password)
			: DatabaseEngine() {
			this->userEmail = userEmail;
			this->userFirstName = userFirstName;
			this->userLastName = userLastName;
		}

		string createConnection(string db, string user, string pwd,
							 string host, string port) {

			connection_string = "dbname=" + db + " user=" + user + " password=" + pwd + " host=" + host + " port=" + port;
			return connection_string;
		}
			
		vector<UserOrm> getAllUsers() {

			try {
				result query_result = getAll("SELECT * from app_users");
				vector <UserOrm> objects;
				for (result::const_iterator c = query_result.begin(); c != query_result.end(); ++c) {
					UserOrm object_data = UserOrm(to_string(c[2]), to_string(c[3]), to_string(c[0]), to_string(c[4]));
					cout << to_string(c[2]) << " " << to_string(c[3]) << " " << to_string(c[0]) << " " << to_string(c[4]) << endl;
					objects.push_back(object_data);
				}
				return objects;
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return {};
			}
		}

		UserOrm getSingleUser(map<string, string> searchQuery) {

			try {
				result query_result = getOne("SELECT * from app_users WHERE "+this->getFilters(searchQuery));
				UserOrm object;
				for (result::const_iterator c = query_result.begin(); c != query_result.end(); ++c) {
					object = UserOrm(to_string(c[2]), to_string(c[3]), to_string(c[0]), to_string(c[4]));
				}
				return object;
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return {};
			}
		}

		int createUser() {

			try {
				string createFilter = this->createUserFilter();
				return create(createFilter);
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return -1;
			}
		}

		int deleteUser(UserOrm user) {

			try {
				string deleteFilter = this->deleteUserFilter();
				return  remove(deleteFilter);
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return -1;
			}
		}

		result filter(string query) {

			return getAll(query);
		}

		int customCreate(string query) {

			return create(query);
		}

		string getString() {

			return "{useremail: " + this->userEmail+", firstname:"+this->userFirstName+ ", lastname:"+this->userLastName +"}";
		}
	private:

		string getFilters(map <string, string> mapQuery) {

			map<string, string>::iterator it;
			string query;
			for (it = mapQuery.begin(); it != mapQuery.end(); it++)
			{
				query += it->first + " = " + it->second;
			}
			return query;
		}

		string createUserFilter() {

			string create_query = "INSERT INTO app_users (FirstName, LastName, Email, password) VALUES";
			create_query += "('" + this->userFirstName + "'" + ",'" + this->userLastName + "'" + \
				",'" + this->userEmail+"'," + "'Ab112233')";
			cout << create_query << endl;
			return create_query;
		}

		string deleteUserFilter() {

			string delete_filter = "DELETE from app_users WHERE Email = "+this->userEmail;
			return delete_filter;
		}
};

class MyPinionObects : public DatabaseEngine {

	public:

		string name;
		string description;
		string type;
		UserOrm user;
		
		MyPinionObects(void){}
		
		MyPinionObects(string connection)
			:DatabaseEngine(connection)
		{

		}

		MyPinionObects(string name, string description, string type, UserOrm user, string connection)
			:DatabaseEngine(connection)
		{
			this->name = name;
			this->description = description;
			this->type = type;
			this->user = user;
		}

		MyPinionObects(string name, string description, string type, UserOrm user)
		{
			this->name = name;
			this->description = description;
			this->type = type;
			this->user = user;
		}

		vector <MyPinionObects> getPinions(map<string, string> filters) {

			string sql_q = "SELECT * FROM app_opinions WHERE " + this->getPinionsFilter(filters);
			try {
				result query_result = getAll(sql_q);
				vector <MyPinionObects> objects;
				for (result::const_iterator c = query_result.begin(); c != query_result.end(); ++c) {
					MyPinionObects object_data = MyPinionObects(to_string(c[0]), to_string(c[1]), to_string(c[2]), to_string(c[3]));
					cout << to_string(c[2]) << " " << to_string(c[3]) << " " << to_string(c[0]) << " " << to_string(c[4]) << endl;
					objects.push_back(object_data);
				}
				return objects;
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return {};
			}
		}

		int createPinions() {

			try {
				string createFilter = this->createPinionsFilter();
				return create(createFilter);
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return -1;
			}
		}

		result filter(string query) {

			return getAll(query);
		}

		int customCreate(string query) {

			return create(query);
		}

	private:

		string getPinionsFilter(map <string, string> filters) {

			map<string, string>::iterator it;
			string query;
			for (it = filters.begin(); it != filters.end(); it++)
			{
				query += it->first + " = " + it->second;
			}
			return query;
		}

		string createPinionsFilter() {

			string create_query = "INSERT INTO app_pinions (name, description, type, user) VALUES";
			create_query += "('" + this->name + "'" + ",'" + this->description + "'" + \
				",'" + this->type + "'," + "'Ab112233')";
			cout << create_query << endl;
			return create_query;
		}
};

class MyPinionQuestions : DatabaseEngine {

	public:

		string question;
		string options;
		MyPinionObects myPinion;

		MyPinionQuestions(void){}
		MyPinionQuestions(string connection)
			:DatabaseEngine(connection) {

		}
		MyPinionQuestions(string question, string options, MyPinionObects myPinion, string connection)
			:DatabaseEngine(connection)
		{
			this->question = question;
			this->options = options;
			this->myPinion = myPinion;
		}
		MyPinionQuestions(string question,string options, MyPinionObects myPinion)
		{
			this->question = question;
			this->options = options;
			this->myPinion = myPinion;
		}

		vector <MyPinionQuestions> getmPinionsQs(map<string, string> filters) {

			string sql_q = this->getPinionsQsFilter(filters);
			try {
				result query_result = getAll(sql_q);
				vector <MyPinionQuestions> objects;
				for (result::const_iterator c = query_result.begin(); c != query_result.end(); ++c) {
					MyPinionQuestions object_data = MyPinionQuestions(to_string(c[0]), to_string(c[1]), to_string(c[2]));
					cout << to_string(c[2]) << " " << to_string(c[3]) << " " << to_string(c[0]) << " " << to_string(c[4]) << endl;
					objects.push_back(object_data);
				}
				return objects;
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return {};
			}
		}

		int createOpnionQs() {

			try {
				string createFilter = this->createPinionsFilter();
				return create(createFilter);
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return -1;
			}
		}

		result filter(string query) {

			return getAll(query);
		}

		int customCreate(string query) {

			return create(query);
		}

	private:

		string getPinionsQsFilter(map<string, string> filters){

			map<string, string>::iterator it;
			string query;
			for (it = filters.begin(); it != filters.end(); it++)
			{
				query += it->first + " = " + it->second;
			}
			return query;
		}

		string createPinionsFilter() {

			string create_query = "INSERT INTO app_pinions_questions (question, options, opnion) VALUES";
			create_query += "('" + this->question + "'" + ",'" + this->options + "'" + \
				",'" + this->myPinion.name + "')";
			cout << create_query << endl;
			return create_query;
		}
};

class MyPinionPermissions : DatabaseEngine {

	public:

		UserOrm user;
		MyPinionObects myPinion;
		int permission;
		string email;

		MyPinionPermissions(void){}
		MyPinionPermissions(string connection) 
			:DatabaseEngine(connection)
		{}
		MyPinionPermissions(UserOrm user, MyPinionObects myPinion, int permission, string email, string connection)
			:DatabaseEngine(connection) {

			this->user = user;
			this->myPinion = myPinion;
			this->permission = permission;
			this->email = email;
		}

		MyPinionPermissions(UserOrm user, MyPinionObects myPinion, int permission, string email){

			this->user = user;
			this->myPinion = myPinion;
			this->permission = permission;
			this->email = email;
		}

		vector<MyPinionPermissions> getPinionPermissions(map <string, string> filters) {

			string sql_q = this->getPinionPermissionFilter(filters);
			try {
				result query_result = getAll(sql_q);
				vector <MyPinionPermissions> objects;
				for (result::const_iterator c = query_result.begin(); c != query_result.end(); ++c) {
					MyPinionPermissions object_data = MyPinionPermissions(to_string(c[0]), to_string(c[1]), (c[2]).as<int>(), to_string(c[3]));
					cout << to_string(c[2]) << " " << to_string(c[3]) << " " << to_string(c[0]) << " " << to_string(c[4]) << endl;
					objects.push_back(object_data);
				}
				return objects;
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return {};
			}
		}

		int createPinionPermission() {

			try {
				string createFilter = this->createPinionPermissionFilter();
				return create(createFilter);
			}
			catch (const std::exception& e) {
				cerr << e.what() << std::endl;
				return -1;
			}
		}

		int customCreate(string query) {

			return create(query);
		}

		result filter(string query) {

			return getAll(query);
		}

	private:

		string getPinionPermissionFilter(map<string, string> filters) {

			map<string, string>::iterator it;
			string query;
			for (it = filters.begin(); it != filters.end(); it++)
			{
				query += it->first + " = " + it->second;
			}
			return query;
		}

		string createPinionPermissionFilter() {

			string create_query = "INSERT INTO app_pinions_permissions (user, mypinion, permission, email) VALUES";
			create_query += "('" + this->user.userEmail + "'" + ",'" + this->myPinion.name + "'" + \
				"," + to_string(this->permission)+ ",'"+ this->email + "')";
			cout << create_query << endl;
			return create_query;
		}

};

template <class E>
class Value {

	public:

		E value;

		template <class E>
		Value(E value) {
			this->value = value;
		}
		template <class E>
		E getValue() {

			return this->value;
		}
};

template <class T>
class Serializer {

	public:

		T instance;
		bool many = false;
		vector<T> instances;
		json serialized_data;

		Serializer(T instance) {
			this->instance = instance;
			this->initializeSetup();
		}
		Serializer(vector<T> instances, bool many) {
			this->instances = instances;
			this->many = many;
			this->initializeSetup();
		}

	private:

		void initializeSetup() {

			string objectType = typeid(T).name();
			cout << objectType << endl;
			map<string, Value> objectData;
			if (objectType.compare("UserOrm")) {
				Value  firstName = Value(instance.userFirstName);
				Value lastName = Value(instance.userLastName);
				Value  email = Value(instance.userEmail);
				int a = 2;
				Value id = Value(a);
				objectData["firstname"] = firstName.getValue();
				objectData["lastname"] = lastName.getValue();
				objectData["email"] = email.getValue();
				objectData["id"] = id.getValue();
				//objectData["id"] = Value(1);
				map<string, string>::iterator it;
				//for (it = objectData.begin(); it != objectData.end(); it++)
				//{
				//	//query += it->first + " = " + it->second;
				//	cout << it->first << " : " << it->second <<endl;
				//}
				cout << instance.getString() << endl;
				this->serialized_data = json::parse(instance.getString());
				cout << this->serialized_data << endl;
			}

		}



};


int main(int argc, char* argv[]) {

	string connection_string = "dbname = testDB  user = xxsddw password = yxdswds host = localhost port = 5432";
	//UserOrm userData = UserOrm("Aneel", "Ahmed", "aneel@wdwds.com");
	//userData.createConnection("testDB", "xxsddw", "yxdswds", "localhost", "5432");
	UserOrm userData = UserOrm(connection_string);
	vector<UserOrm> users = userData.getAllUsers();
	//cout << users.front().getString() << endl;
	//userData.createUser();
	Serializer serialize = Serializer(users.front());
}