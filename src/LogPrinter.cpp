#include "LogPrinter.h"


string LogPrinter::idHeader(int id) {
	string outS = to_string(id);
	outS += ": ";
	return outS;
}

// Take a string, write in file
void LogPrinter::writeInFile(string s, string file_name) {
  
  ofstream fout;
  fout.open(file_name, ios::app); // Append to file

  // Write to the file
  fout << s;
 
  // Close the file
  fout.close();

}

// Get raw time, return well formatted UTC time string
string LogPrinter::rawTimeCooker(time_t rawtime) {
  struct tm * timeinfo;

  timeinfo = gmtime(&rawtime); // UTC
  return asctime(timeinfo);
}



//   Upon receiving a new request, your proxy should assign it a unique id (ID), and print the
// ID, time received (TIME), IP address the request was received from (IPFROM) and the
// HTTP request line (REQUEST) of the request in the following format:

// ID: "REQUEST" from IPFROM @ TIME
void LogPrinter::newRequest(int id, time_t time, string ipStr, string requestStr) {
	string out;

	out += this->idHeader(id); // ID header
	out += "\"";
	out += requestStr;
	out += "\" from ";
	out +=ipStr;
	out += " @ ";
	out += this->rawTimeCooker(time);

	this->writeInFile(out, LOG_FILE_NAME); // Write in file

}

// If the request is a GET request, your proxy should check its cache, and print one of the
// following:
// ID: not in cache
// ID: in cache, but expired at EXPIREDTIME
// ID: in cache, requires validation
// ID: in cache, valid
void LogPrinter::checkCache(int id, int ctl_word, time_t ExpireTime) {
	string out;

	out += this->idHeader(id); // ID header
	
	if(ctl_word == NOT_IN) {
		out += "not in cache";
	}
	else {
		out += "in cache, ";
		switch (ctl_word) { 
    	case EXPIRE:	
    	out += "but expired at ";
    	out += this->rawTimeCooker(ExpireTime);
        break;
    	case NEED_VALID: 
        out += "requires validation";
        break;
        case VALID: 
        out += "valid";
        break;
		}
	}

	if(out.back() != '\n') {
		out += "\n";
	}

	this->writeInFile(out, LOG_FILE_NAME); // Write in file

}

// If your proxy needs to contact the origin server about the request, it should print the
// request it makes to the origin server:

// ID: Requesting "REQUEST" from SERVER
void LogPrinter::Request(int id, string ReqLine, string ServerName) {
	string out;

	out += this->idHeader(id); // ID header
	out += "Requesting \"";
	out += ReqLine;
	out += "\" from ";
	out += ServerName;
	
	if(out.back() != '\n') {
		out += "\n";
	}

	this->writeInFile(out, LOG_FILE_NAME); // Write in file
}

// Later, when it receives the response from the origin server, it should print:

// ID: Received "RESPONSE" from SERVER
void LogPrinter::Receive(int id, string ResLine, string ServerName) {
	string out;

	out += this->idHeader(id); // ID header
	out += "Received \"";
	out += ResLine;
	out += "\" from ";
	out += ServerName;
	
	if(out.back() != '\n') {
		out += "\n";
	}

	this->writeInFile(out, LOG_FILE_NAME); // Write in file
}

//Here, REQUEST and RESPONSE are the request line and response line (first line in the
//message), and SERVER is the server name.
//----------------------------------------------end---------------------------------------------------

//If your proxy receives a 200-OK in response to a GET request, it should print one of the
//following:

// @Overload
// ID: cached, expires at EXPIRES
// ID: cached, but requires re-validation
void LogPrinter::p200OK(int id, int ctl_word, time_t ExpireTime) {
	string out;

	out += this->idHeader(id); // ID header
	switch (ctl_word) { 
		// case NOT_CACHEABLE:
		// out += "not cacheable because ";
		// out += reason;
		// break;
    	case EXPIRE:	
    	out += "cached, expires at ";
    	out += this->rawTimeCooker(ExpireTime);
        break;
    	case NEED_VALID: 
        out += "cached, but requires re-validation";
        break;

		}
	
	

	if(out.back() != '\n') {
		out += "\n";
	}

	this->writeInFile(out, LOG_FILE_NAME); // Write in file
}

// @Overload
// ID: not cacheable because REASON
void LogPrinter::p200OK(int id, int ctl_word, string reason) {
	string out;

	out += this->idHeader(id); // ID header
	switch (ctl_word) { 
		case NOT_CACHEABLE:
		out += "not cacheable because ";
		out += reason;
		break;
    	// case EXPIRE:	
    	// out += "cached, expires at ";
    	// out += this->rawTimeCooker(ExpireTime);
     //    break;
    	case NEED_VALID: 
        out += "cached, but requires re-validation";
        break;

		}
	
	

	if(out.back() != '\n') {
		out += "\n";
	}

	this->writeInFile(out, LOG_FILE_NAME); // Write in file
}
//----------------------------------------------end---------------------------------------------------

// Whenever your proxy responds to the client, it should log:

// ID: Responding "RESPONSE"
void LogPrinter::Respond(int id, string response) {
	string out;
	out += this->idHeader(id); // ID header

	out += "Responding \"";
	out += response;

	out += "\"";

	if(out.back() != '\n') {
		out += "\n";
	}

	this->writeInFile(out, LOG_FILE_NAME); // Write in file

}

//When your proxy is handling a tunnel as a result of 200-OK, it should log (in addition to
//all other normal logging) when the tunnel closes with:

// ID: Tunnel closed
void LogPrinter::tunnelClosed(int id) {
	string out;
	out += this->idHeader(id); // ID header
	out += "Tunnel closed";

	if(out.back() != '\n') {
		out += "\n";
	}

	this->writeInFile(out, LOG_FILE_NAME); // Write in file
}
