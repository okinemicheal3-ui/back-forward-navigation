#include <iostream>
#include <string>
#include <stack>
#include "httplib.h" 

using namespace std;

// --- Browser History Logic ---
class BrowserHistory {
private:
    stack<string> backStack;
    stack<string> forwardStack;
    string currentPage;

public:
    BrowserHistory() {
        currentPage = "Home Page";
    }

    void visit(const string& url) {
        if (url.empty()) return;
        // Push current page to back history
        backStack.push(currentPage);
        // Clear forward history because we made a new action
        while (!forwardStack.empty()) forwardStack.pop();
        // Set new current page
        currentPage = url;
    }

    void goBack() {
        if (backStack.empty()) return; // Can't go back
        // Push current page to forward history
        forwardStack.push(currentPage);
        // Pop the last page from back history and make it current
        currentPage = backStack.top();
        backStack.pop();
    }

    void goForward() {
        if (forwardStack.empty()) return; // Can't go forward
        // Push current page to back history
        backStack.push(currentPage);
        // Pop the last page from forward history and make it current
        currentPage = forwardStack.top();
        forwardStack.pop();
    }

    string getCurrentPage() const { return currentPage; }
};

// Global browser instance
BrowserHistory browser;

int main() {
    httplib::Server svr;

    // 1. Serve the HTML Frontend (Simulating a Browser)
    svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
        string html = R"HTML(
            <!DOCTYPE html>
            <html>
            <head>
                <title>C++ Browser History</title>
                <style>
                    body { font-family: sans-serif; max-width: 600px; margin: 40px auto; text-align: center; }
                    .browser { border: 2px solid #333; border-radius: 8px; padding: 20px; background: #f4f4f4; }
                    .toolbar { margin-bottom: 20px; }
                    .toolbar button { padding: 8px 15px; margin: 0 5px; cursor: pointer; font-size: 16px; }
                    #address-bar { width: 60%; padding: 8px; font-size: 16px; margin-right: 10px; }
                    .page { border: 1px solid #ccc; padding: 50px; background: white; font-size: 24px; margin-top: 20px; border-radius: 4px; }
                </style>
            </head>
            <body>
                <h2>🌐 Mini Browser</h2>
                <div class="browser">
                    <div class="toolbar">
                        <button onclick="send('/back')">⬅ Back</button>
                        <button onclick="send('/forward')">Forward ➡</button>
                    </div>
                    <div>
                        <input type="text" id="address-bar" placeholder="Enter URL (e.g., google.com)">
                        <button onclick="send('/visit?url=' + document.getElementById('address-bar').value)">Go</button>
                    </div>
                    <div class="page" id="display">Home Page</div>
                </div>
                <script>
                    function send(endpoint) {
                        fetch(endpoint)
                            .then(res => res.text())
                            .then(data => { 
                                document.getElementById('display').innerText = data; 
                                // Update address bar to match current page
                                document.getElementById('address-bar').value = (data === "Home Page") ? "" : data;
                            });
                    }
                </script>
            </body>
            </html>
        )HTML";
        res.set_content(html, "text/html");
    });

    // 2. API Endpoints for Stack Operations
    svr.Get("/visit", [](const httplib::Request &req, httplib::Response &res) {
        if (req.has_param("url")) browser.visit(req.get_param_value("url"));
        res.set_content(browser.getCurrentPage(), "text/plain");
    });

    svr.Get("/back", [](const httplib::Request &, httplib::Response &res) {
        browser.goBack();
        res.set_content(browser.getCurrentPage(), "text/plain");
    });

    svr.Get("/forward", [](const httplib::Request &, httplib::Response &res) {
        browser.goForward();
        res.set_content(browser.getCurrentPage(), "text/plain");
    });

    cout << "Server starting on port 8080..." << endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}