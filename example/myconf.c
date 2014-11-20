#include "conf.h"

int main()
{
        struct conf_s *conf;

        conf = conf_init("./tt.ini");
        if (conf == NULL) return -1;


        printf("port:%s\n", conf_get(conf, "comm", "port"));
        printf("port:%s\n", conf_get(conf, "tt", "tt"));

        conf_free(conf);
        return 0;
}


