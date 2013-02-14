/*
 * KMake.c
 *
 *  Created on: 2009-9-27
 *      Author: Nick.Xu
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <io.h>
#include <dos.h>
#include <windows.h>
#include <common.c>

void file_copy(char *, char *);
void file_copy(char *sourceFile, char *destFile) {
	if (access(destFile, 0) == 0) {
		unlink(destFile);
	}

	FILE *fp1;
	FILE *fp2;
	fp1 = fopen(sourceFile, "r");
	fp2 = fopen(destFile, "w+");
	char fileContent[10240];
	char *allContent = "";
	//printf("#1:%s#\n#2:%s#\n", sourceFile, destFile);
	if (fp1 != NULL) {
		while (fgets(fileContent, 128, fp1) != NULL) {
			allContent = str_join(allContent, fileContent);
		}
		fputs(allContent,fp2);
		//fputs(str_replace(allContent, "//interrupt", "interrupt"), fp2);
	}
	free(fileContent);
	fclose(fp1);
	fclose(fp2);
}

char* getFiles(char *, char *);
char* getFiles(char *filePath, char *fileType) {

	const char *fileTemplate = "File 1,1,<..\\%s><%s> 0x0 \r\n";
	static char fileList[2000] = "";

	//列出所有C文件
	struct _finddata_t fileInfo;
	char ss[200] = "";
	strcat(ss, filePath);

	strcat(ss, fileType);
	//printf("file:%s\r\n",ss);
	long i = _findfirst(ss, &fileInfo);

	free(ss);
	if (i > 0) {
		sprintf(fileList, fileTemplate, fileInfo.name, fileInfo.name);
		//printf("ac%s",fileType);
	}

	while (!_findnext(i, &fileInfo)) {
		sprintf(ss, fileTemplate, fileInfo.name, fileInfo.name);
		strcat(fileList, ss);
	}
	free(ss);

	return fileList;
}

char * left(char *dst, char *src, int n) {
	char *p = src;
	char *q = dst;
	int len = strlen(src);
	if (n > len)
		n = len;

	while (n--)
		*(q++) = *(p++);
	*(q++) = '\0';
	return dst;
}

int main(argc, argv)
	int argc;char *argv[]; {
	printf("KeilMake Ver 2.0 for MDK 3.x \n");
	//const char *fileTemplate = "File 1,1,<..\\%s><%s> 0x0 \r\n";

	//文件列表操作
	//char *fileList = "";
	//char ss[100];

	if (argc < 3) {
		printf("keilmake.exe KeilUvExePath templateUVFileName codePath");
		return 0;
	}

	//printf("#\r\n%s\r\n#",argv[3]);
	FILE *fp;

	fp = fopen(argv[2], "r");
	char *fileContent = "";
	char content[128];
	char fileList[2000] = "";
	char projectPath[128] = "";
	char projectFile[200] = "";
	char projectFileOpt[200] = "";
	char *sourceOpt="";
	char *templateUV = argv[2];
	char *strCmd="";
	char strOutput[MAX_PATH]="";
	char * findResult;
	templateUV = str_replace(templateUV, "/", "\\");
	if (fp != NULL) {
		printf("reading templateFile %s...\n", argv[2]);
		//打开模板文件
		while (fgets(content, 128, fp) != NULL) {
			//printf("1");
			fileContent = str_join(fileContent, content);
			//fileContent = str_join(fileContent, content);
		}
		fclose(fp);

		//strncpy(content,argv[2],strlen(argv[2])- strlen(strrchr(argv[2],'\\')));
		left(projectPath, argv[2], strlen(argv[2]) - strlen(strrchr(argv[2],
				'\\')));

		strcat(fileList, getFiles(projectPath, "\\..\\*.c"));
		//strcat(fileList, getFiles(projectPath, "\\..\\*.h"));
		//strcpy(content,"abcdefg");
		//printf("ff:%s",fileList);

		//开始替换
		fileContent = str_replace(fileContent, "{CodeList}", fileList);

		//char *t= strrchr(argv[1],'\\');
		//printf(fileContent);
		strcat(projectFileOpt, projectPath);
		strcat(projectFileOpt, "\\_keilmake.opt");
		sourceOpt=str_replace(argv[2],".Uv2",".opt");
		if (access(sourceOpt, 0) != 0) {
			printf("Can not find %s",sourceOpt);
			return 0;
		}

		file_copy(sourceOpt,projectFileOpt);

		strcat(projectFile, projectPath);
		strcat(projectFile, "\\_keilmake.Uv2");
		printf("writing file [");
		printf(projectFile);
		printf("] ...\n");
		fp = fopen(projectFile, "w");
		if (fp != NULL) {
			fputs(fileContent, fp);
		}
		fclose(fp);

		getcwd(strOutput,MAX_PATH);
		strcat(strOutput,"\\build.txt");

		strCmd = "";
		char *strBuildOption="";

		strCmd = str_join(argv[1], " -r ");
		strCmd = str_join(strCmd, projectFile);
		strCmd = str_join(strCmd, " -o\"");
		strCmd = str_join(strCmd, strOutput );
		strCmd = str_join(strCmd, "\"");

		if (argc > 3 && strcmp(argv[3], "debug") == 0) {
			strBuildOption=" -d ";
			if (access(strOutput, 0) == 0) {
				unlink(strOutput);
			}
			system(strCmd);
			fileContent = "";
			fp = fopen(strOutput, "r");
			if (fp != NULL) {
				while (fgets(content, 128, fp) != NULL) {
					fileContent = str_join(fileContent, content);
				}
			}
			fclose(fp);
		    findResult = strstr(fileContent, "Target not created");
			printf("%s", fileContent);
			if (findResult) {
				return 0;
			}
			Sleep(500);
		}
		else if(argc > 3 && strcmp(argv[3], "upload") == 0){
			strBuildOption=" -f ";
			if (access(strOutput, 0) == 0) {
				unlink(strOutput);
			}
			system(strCmd);
			fileContent = "";
			fp = fopen(strOutput, "r");
			if (fp != NULL) {
				while (fgets(content, 128, fp) != NULL) {
					fileContent = str_join(fileContent, content);
				}
			}
			fclose(fp);
		    findResult = strstr(fileContent, "Target not created");
			printf("%s", fileContent);
			if (findResult) {
				return 0;
			}
			Sleep(500);
		}
		else {
			strBuildOption=" -r ";
		}


		strCmd = "";
		strCmd = str_join(argv[1], strBuildOption);
		strCmd = str_join(strCmd, projectFile);
		strCmd = str_join(strCmd, " -o\"");
		strCmd = str_join(strCmd, strOutput );
		strCmd = str_join(strCmd, "\"");
		//c:\\build.txt\"

		//rintf("%s\n",);


		if (access(strOutput, 0) == 0) {
			unlink(strOutput);
		}
		system(strCmd);

		fileContent = "";
		fp = fopen(strOutput, "r");
		if (fp != NULL) {
			while (fgets(content, 128, fp) != NULL) {
				fileContent = str_join(fileContent, content);
			}
		}
		fclose(fp);
		unlink(strOutput);
		printf("%s", fileContent);
		printf("Powered by ShowVI.com. \n");
		printf("q@wholuck.com. \r\n");
	} else {
		printf("can't find ");
		printf(argv[2]);
	}

	return 0;
}

