#include "readcsv.h"

int main()
{
        struct csv_s *csv;
        char *fdata;
        size_t len = 0;
        printf("begin:%lld\n", tm64());
        fdata = filedata("hero.csv", &len);
        if (fdata==NULL) perror("filedata:");

        printf("file length:%x\n", len);
        csv = parse_csv(fdata, len);
        printf("data:%s, lines:%d, columns:%d\n", csv_data(csv, 2, 9), csv->records, csv->columns);
        printf("data:%s, lines:%d, columns:%d\n", csv_data_f(csv, 100, "intro"), csv->records, csv->columns);
        printf("data:%s, lines:%d, columns:%d\n", csv_data_id(csv, "10059", 2), csv->records, csv->columns);
        printf("data:%s, lines:%d, columns:%d\n", csv_data_fid(csv, "10059", "intro"), csv->records, csv->columns);
        //hexdump(fdata, len);
        free(fdata);
        free_csv(csv);
        printf("end  :%lld\n", tm64());
}

