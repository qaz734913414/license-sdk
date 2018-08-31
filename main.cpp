#include <iostream>
#include <string>
#include "lib/sl_2d_face_license.h"
#include <unistd.h>

using namespace std;

int main(int argc, char** argv)
{
	while(1)
	{
		sleep(1);
		cout << Sl2DFaceLisence("/tmp") << endl;
	}

	
	/*
	if (argc != 4)
	{
		cout << "usage:" << endl;
		cout << "----encrypt: " << endl;
		cout << "-e src_file dst_file" << endl;
		cout << "----decrypt: " << endl;
		cout << "-d src_file dst_file" << endl;
		system("pause");
		return 0;
	}
	if (strcmp(argv[1], "-e") == 0)
	{
		FILE * outfile, *infile;
		outfile = fopen(argv[3], "ab");
		infile = fopen(argv[2], "rb");

		if (outfile == NULL || infile == NULL)
		{
			printf("%s, %s", argv[1], "not exit/n");
			exit(1);
		}

		string res;

		int rc;
		string r;
		char buf[1024] = { 1024 };
		while ((rc = fread(buf, sizeof(char), 1024, infile)) != 0)
		{
			r = AESEncryptRaw(string(buf, rc));
			res += r;
			//fwrite(r.c_str(), sizeof(char), r.length(), outfile);
		}
		string bsRes;
		Base64::Encode(res, &bsRes);
		fwrite(bsRes.c_str(), sizeof(char), bsRes.length(), outfile);
		fclose(infile);
		fclose(outfile);
	}
	if (strcmp(argv[1], "-d") == 0)
	{
		FILE * outfile, *infile;
		outfile = fopen(argv[3], "ab");
		infile = fopen(argv[2], "rb");

		if (outfile == NULL || infile == NULL)
		{
			printf("%s, %s", argv[1], "not exit/n");
			exit(1);
		}

		string cont;

		int rc;
		string r;
		char buf[1024] = { 1024 };
		while ((rc = fread(buf, sizeof(char), 1024, infile)) != 0)
		{
			cont += string(buf, rc);
			//r = AESDecryptRaw(string(buf, rc));
			//fwrite(r.c_str(), sizeof(char), r.length(), outfile);
		}
		cout << cont << endl;
		string res = AESDecrypt(cont);
		fwrite(res.c_str(), sizeof(char), res.length(), outfile);

		fclose(infile);
		fclose(outfile);
	}
*/
	return 0;
}
