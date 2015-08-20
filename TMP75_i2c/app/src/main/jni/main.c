//
// Created by Tony.CW.Wu on 2015/8/19.
//

#include "main.h"

#include <jni.h>
#include <string.h>
#include <linux/i2c.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <android/log.h>

#define I2C_RDWR        0x0707
#define I2C_SLAVE       0x0703  /* Use this slave address */
#define TMP75_4Fh_ADDR 0x9E>>1



/*********定義struct i2c_rdwr_ioctl_data和struct i2c_msgg，要和內核一致*******/
struct i2c_msgg {
    unsigned short addr;
    unsigned short flags;
    unsigned short len;
    unsigned char *buf;
};

struct i2c_rdwr_ioctl_data {
    struct i2c_msgg *msgs;
    int nmsgs;
};


struct i2c_rdwr_ioctl_data i2c_data;

char* read_TMP75(int fd, char *data, int size, int offset) {
    int ret;

    struct i2c_msgg msg[2];
    char tmp[2];

    /*
     * Two operation is necessary for read operation.
     * First determine offset address
     * , and then receive data.
     */
    i2c_data.msgs = msg;
    i2c_data.nmsgs = 1; // two i2c_msg
//
//    tmp[0] = offset;
//    i2c_data.msgs[0].addr = TMP75_4Fh_ADDR;
//    i2c_data.msgs[0].flags = 0; // write
//    i2c_data.msgs[0].len = 1; // only one byte
//    i2c_data.msgs[0].buf = (__u8 *) tmp;


    /*
     * Second, read data from the EEPROM
     */
    i2c_data.msgs[0].addr = TMP75_4Fh_ADDR;
    i2c_data.msgs[0].flags = I2C_M_RD; // read command
    i2c_data.msgs[0].len = 2;
    i2c_data.msgs[0].buf = (__u8 *) data; //read temperature value to data

    ret = ioctl(fd, I2C_RDWR, &i2c_data);
    if (ret < 0) {
        perror("read data fail\n");

        return i2c_data.msgs[1].buf;
    }

//    int i = 0;
//    //print log in DDMS
//	for (i = 0; i < 2; i++) {
//		__android_log_print(ANDROID_LOG_INFO, "JNIMsg",
//				"i2c_data.msgs[1].buf[%d] =  %x", i, i2c_data.msgs[1].buf[i]);
//
//		__android_log_print(ANDROID_LOG_INFO, "JNIMsg", "data[%d] =  %x", i,
//				data[i]);
//	}

    return ret;

}

int g_i2cFile;


JNIEXPORT jbyteArray JNICALL
Java_com_darwinprecisions_tmp75_1i2c_MainActivity_i2cReadTemp(JNIEnv *env, jobject instance) {

    // TODO


    g_i2cFile = open("/dev/i2c-2", O_RDWR);

    if (g_i2cFile < 0) {
        perror("i2cOpen");
        return (*env)->NewStringUTF(env, "i2cOpen error");
    }
    if (ioctl(g_i2cFile, I2C_SLAVE, 0x4f) < 0) {
        perror("i2cSetAddress");
        return (*env)->NewStringUTF(env, "i2cSetAddress error");
    }


    char *buf_rcv;
    buf_rcv = (char*) malloc(2);

    read_TMP75(g_i2cFile, buf_rcv, sizeof(buf_rcv), 0x00);
    close(g_i2cFile); // Array to fill with data

    jcharArray Array;
    Array = (*env)->NewCharArray(env, sizeof(buf_rcv));// Init  java byte array
    (*env)->SetCharArrayRegion(env, Array, 0, sizeof(buf_rcv), buf_rcv);

    return Array;

}

