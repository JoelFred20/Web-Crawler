# Web-Crawler

Web Crawler

1.The code starts with including necessary libraries and headers such as <bits/stdc++.h>, <curl/curl.h>, <regex>, and <chrono>.

2.The get_page function is responsible for using the CURL library to download the HTML content of a given URL and save it to a file. It takes the URL and the name of the file as input.

3.The extract_hyperlinks function is used to extract all the hyperlinks present in an HTML file. It takes the name of the HTML file as input and returns a vector of strings representing the extracted hyperlinks.

4.The cleanUp function is used to clean up the extracted hyperlinks by removing any unnecessary characters or spaces. It takes a vector of strings representing the hyperlinks as input and modifies it in-place.

5.The parseRobotsTxt function is responsible for downloading and parsing the "robots.txt" file for the given website. It populates the robotsTxtRules map with the rules specified in the "robots.txt" file for different user agents. It also populates the crawlDelays map with the crawl delay values for different user agents.

6.The isAllowed function is used to check if a given URL is allowed to be crawled based on the rules specified in the "robots.txt" file. It returns a boolean value indicating whether the URL is allowed or not.

7.The saveLinksToFile function is used to save a vector of links to a file. It takes the vector of links and the file path as input and appends the links to the file.

8.The dfs_crawler function is the main function that implements the DFS crawling algorithm. It takes the starting URL, file path, depth, and execution count as input. It starts by visiting the URL and downloading the HTML content. Then, it extracts the hyperlinks from the HTML content and cleans them up. It checks if each extracted link is allowed to be crawled based on the "robots.txt" rules and whether it has been visited or saved before. If a link is allowed and not visited or saved before, it adds it to the list of new links and saves it to the links file. It then recursively calls the dfs_crawler function for each new link, incrementing the depth by 1. The function also handles exceptions and prints error messages in case of any errors.

9.Finally, the main function prompts the user to enter a starting URL and calls the parseRobotsTxt function to parse the "robots.txt" file for the website. It then generates a unique links file name based on the execution count and calls the dfs_crawler function to start the crawling process.

In summary, this code implements a web crawler that uses DFS to crawl and download web pages from a given starting URL, while respecting the rules specified in the "robots.txt" file for the website.
