#include <bits/stdc++.h>
#include <curl/curl.h>
#include <regex>
#include <chrono>
using namespace std;

void get_page(const char* url, const char* file_name)
{
  CURL* easyhandle = curl_easy_init();
  if (!easyhandle)
  {
    throw runtime_error("Failed to initialize CURL");
  }

  curl_easy_setopt(easyhandle, CURLOPT_URL, url);
  FILE* file = fopen(file_name, "w");
  if (!file)
  {
    curl_easy_cleanup(easyhandle);
    throw runtime_error("Failed to open file for writing");
  }

  curl_easy_setopt(easyhandle, CURLOPT_WRITEDATA, file);
  CURLcode res = curl_easy_perform(easyhandle);
  if (res != CURLE_OK)
  {
    fclose(file);
    curl_easy_cleanup(easyhandle);
    throw runtime_error("Failed to download the URL");
  }

  fclose(file);
  curl_easy_cleanup(easyhandle);
}

vector<string> extract_hyperlinks(string html_file_name)
{
  string html;
  ifstream read;
  read.open(html_file_name);
  if (!read)
  {
    throw runtime_error("Failed to open HTML file for reading");
  }

  while (!read.eof())
  {
    if (read.eof()) break;
    char ch;
    read.get(ch);
    html.push_back(ch);
  }
  read.close();

  static const regex hl_regex("<a[^>]*href=\"(.*?)\"[^>]*>", regex_constants::icase);
  vector<string> links;
  smatch match;
  while (regex_search(html, match, hl_regex))
  {
    links.push_back(match[1]);
    html = match.suffix();
  }

  return links;
}

void cleanUp(vector<string>& all_links)
{
  vector<string> final_links;
  for (int i = 0; i < all_links.size(); i++)
  {
    string one_link = all_links[i];
    string cleaned_link = "";
    for (int j = 0; j < one_link.length(); j++)
    {
      if (one_link[j] == ' ' || one_link[j] == 34) break;
      cleaned_link.push_back(one_link[j]);
    }
    if (regex_match(cleaned_link, regex("((http|https)://)(www.)?[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)")))
    {
      final_links.push_back(cleaned_link);
    }
  }
  all_links.clear();
  all_links = final_links;
}

unordered_map<string, bool> visited;

// Add these global variables to store 'robots.txt' information
unordered_map<string, vector<string>> robotsTxtRules;
unordered_map<string, int> crawlDelays;

// Function to parse 'robots.txt' content and populate rules and delays
void parseRobotsTxt(const string& url)
{
  string robotsUrl = url + "/robots.txt";
  const char* filename = "robots.txt";

  get_page(robotsUrl.c_str(), filename); // Download 'robots.txt'

  ifstream file(filename);
  string line;
  string currentAgent;

  while (getline(file, line))
  {
    // Parse 'User-agent' and 'Disallow' directives
    if (line.substr(0, 10) == "User-agent")
    {
      currentAgent = line.substr(12);
      continue;
    }
    if (line.substr(0, 8) == "Disallow")
    {
      robotsTxtRules[currentAgent].push_back(line.substr(10));
    }
    if (line.substr(0, 11) == "Crawl-Delay")
    {
      crawlDelays[currentAgent] = stoi(line.substr(13));
    }
  }
  file.close();
}

// Function to check if a URL is allowed based on 'robots.txt'
bool isAllowed(const string& url)
{
  // Extract the domain from the URL
  size_t startPos = url.find("://") + 3;
  size_t endPos = url.find("/", startPos);
  string domain = url.substr(startPos, endPos - startPos);

  // Check against 'robots.txt' rules
  for (const auto& rule : robotsTxtRules)
  {
    if (rule.first == "*" || rule.first == domain)
    {
      for (const auto& disallow : rule.second)
      {
        if (url.find(disallow) != string::npos)
        {
          return false; // Disallowed by robots.txt
        }
      }
    }
  }
  return true; // Allowed by robots.txt
}

// ... (Previous code)

unordered_set<string> visitedUrls; // Maintain a set of visited URLs

// ... (Other functions)

void saveLinksToFile(const vector<string>& links, const string& file_path)
{
    ofstream outfile(file_path, ios::app); // Open in append mode
    if (!outfile)
    {
        throw runtime_error("Failed to open file for writing");
    }

    for (const string& link : links)
    {
        outfile << link << endl;
    }

    outfile.close();
}

// ... (Previous code)

unordered_set<string> savedLinks; // Maintain a set of saved links

// ... (Previous code)

void dfs_crawler(const char* url, const char* file_path, int depth, int executionCount)
{
    // Generate a unique links file name based on execution count
    stringstream ss;
    ss << "links_" << executionCount << ".txt";
    string links_file = ss.str();

    string s = url;

    if (depth == 4 || visitedUrls.count(s) || visited[url])
        return;

    visitedUrls.insert(s);
    visited[s] = true;

    try
    {
        if (!isAllowed(url))
            return;

        get_page(url, file_path);

        vector<string> allLinksData = extract_hyperlinks(file_path);
        cleanUp(allLinksData);

        sort(allLinksData.begin(), allLinksData.end());

        vector<string> newLinks;
        for (const string& i : allLinksData)
        {
            if (!visited[i] && savedLinks.find(i) == savedLinks.end())
            {
                newLinks.push_back(i);
                savedLinks.insert(i);
            }
        }

        saveLinksToFile(newLinks, links_file);

        for (const string& i : newLinks)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            dfs_crawler(i.c_str(), file_path, depth + 1, executionCount);
        }
    }
    catch (const exception& e)
    {
        cerr << "An error occurred: " << e.what() << endl;
    }
}

int main()
{
    try
    {
        string input_url;
        cout << "Enter the URL to start crawling: ";
        getline(cin, input_url);

        parseRobotsTxt(input_url);

        const char* html_filename = "page.html";

        // Generate a new execution count each time the program runs
        static int executionCount = 0;
        executionCount++;

        dfs_crawler(input_url.c_str(), html_filename, 0, executionCount);
    }
    catch (const exception& e)
    {
        cerr << "An error occurred: " << e.what() << endl;
    }

    return 0;
}
