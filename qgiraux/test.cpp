#include <sstream>
#include <iostream>
#include <map>
#include <fstream>

int main()
{
    int errcode = 400;
    std::map<int, std::string> errorList;

    errorList[errcode] = "this is a test";

        std::stringstream os;

    os << "<!DOCTYPE html> "
    << "<html lang=\"en\"> "
    << "<head>"
    << "<meta charset=\"UTF-8\"> "
    << "<meta name=\"viewport\"" 
    << "content=\"width=device-width, initial-scale=1.0\"> "
    << "<title> "
    << errcode <<  errorList[errcode]
    << "</title> "
    << "<link rel=\"stylesheet\" " 
    << "href=\"style.css\"> "
    << "</head>" 
    << "<body>"
    << "<div class=\"page\"> "
    << "<div class=\"title\"> "
    << "<h1>" << errcode << "</h1> "
    << "</div> "
    << "<div class=\"title\"><p>"
    << errorList[errcode]
    << "</p></div> "
    << "<div class=\"title\">"
    << " <a href=\"index.html\">" 
    << "Go Back to Home "
    << "</a></div> "
    << " <div class=\"bloc\">"
	<< "<img src=\"images/404.png\" alt=" << errcode <<" class=\"resizable-image\">"
	<< "	</div></div></body> </html>";

    std::ofstream file;

    file.open("tmp.html", std::ios::out);
    file << os.str();
    file.close();
}