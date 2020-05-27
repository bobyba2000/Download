#define _CRT_SECURE_NO_WARNINGS
#define HAVE_STRUCT_TIMESPEC
#include<iostream>
#include<stdio.h>
#include<string>
#include<thread>
#include<curl/curl.h>

using namespace std;

// ToDo: Add a make file
// List down all the dependencies
// Have a todo list of features

void threadDown(CURL *curl, long rangeStart, long rangeStop, const char* fileName, bool check)
{
	FILE *fp = fopen(fileName, "wb");
	string range = std::to_string(rangeStart) + '-' + to_string(rangeStop);
	curl_easy_setopt(curl, CURLOPT_RANGE, range);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	fclose(fp);
	check = true;
}

long max1(long a, long b)
{
	if (a > b)return a;
	return b;
}

void* download(void* ptr, int numThread, const char*outfilename) {
	char* url = (char *)ptr;
	// START THE DOWNLOAD HERE
	CURL *curl;
	FILE *fp;
	CURLcode res;
	curl = curl_easy_init();
	
	if (curl)
	{
		fp = fopen(outfilename, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		double x = 0;
		curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &x);

		thread *multiThread;
		if (x == -1)
		{
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			fclose(fp);
		}
		else
		{
			long range = x / numThread;
			multiThread = new thread[numThread];
			bool *check = new bool[numThread];
			string fileName = "download";
			
			//chia cac thread va luu vao cac file con
			for (int i = 0; i < numThread; i++)
			{
				check[i] = false;
				multiThread[i] = thread(threadDown, curl, range*i, max1(range*(i + 1), x), (fileName + to_string(i)).c_str(), check[i]);
				multiThread[i].join();
			}
			
			for (int i = 0; i < numThread; i++)
			{
				//kiem tra xem thread i da duoc tai du chua
				while (!check[i])
				{
				}

				//sao chep cac file vao file dich
				FILE*fp1 = fopen((fileName + to_string(i)).c_str(), "rb");
				const int N = 1000;
				char s[N * 10];
				int count;
				do {
					count = fread(s, sizeof(char), N, fp1);
					fwrite(s, sizeof(char), count, fp);
				} while (count >= sizeof(char)*N);
				fclose(fp1);
			}
			delete multiThread;
			delete check;
			fclose(fp);
		}
	}

	exit(0);
	return NULL;
}

void threadFunc()
{
	int numThread = 1;
	string url = "";
	string command;
	getline(cin, command);
	int found = command.find(string("--url=")) + 6;
	while (found < command.length() && command[found] != ' ')
	{
		url += command[found];
		found++;
	}

	string thread = "";
	found = command.find(string("--thread=")) + 9;
	while (found < command.length() && command[found] != ' ')
	{
		thread += command[found];
		found++;
	}
	numThread = stoi(thread);

	string outfilename = "";
	found = command.find(string("--out=")) + 6;
	while (found < command.length() && command[found] != ' ')
	{
		outfilename += command[found];
		found++;
	}

	void * url_ptr = (void *)static_cast<const void *>(url.c_str());
	download(url_ptr, numThread, outfilename.c_str());
}

int main(int argc, char** argv) {
	thread tid;
	tid = thread(threadFunc);
	HANDLE handle_t1 = tid.native_handle();
	tid.join();

	//Pause and resume download
	while (true)
	{
		string command = "";
		cout << "Go Resume hoac Pause de tiep tuc hoac dung tai: ";
		cin >> command;
		if (command == "Resume")
			ResumeThread(handle_t1);
		else if (command == "Pause")
			SuspendThread(handle_t1);
	}
	return 0;
}