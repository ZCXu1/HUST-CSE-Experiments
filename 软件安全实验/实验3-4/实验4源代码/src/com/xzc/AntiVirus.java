package com.xzc;

import org.apache.commons.io.FileUtils;

import java.io.*;

/**
 * @author XuZichuan
 * @date 2021.6.23
 * @version 1.0.0
 */

public class AntiVirus {
    /**
     * 检查文件是否是病毒或是否被感染 输出文件名称和路径
     * @param file 被检查的文件
     */
    public static void checkFile(File file){
        if (fileContainsString(file,"infectDir")){
            System.out.println("Found Virus!");
            System.out.println("File Name: "+file.getName());
            System.out.println("File Position: "+file.getAbsolutePath());
        }
        if (fileContainsString(file,"infected")){
            System.out.println("Found Infected File!");
            System.out.println("File Name: "+file.getName());
            System.out.println("File Position: "+file.getAbsolutePath());
        }
    }

    /**
     * 检查文件夹下的文件和子文件夹
     * @param dir 被检查的文件夹
     */
    public static void checkDir(File dir){
        if (!dir.isDirectory()){
            return;
        }
        File[] list = dir.listFiles();
        if(list != null) {
            for (File f : list) {
                if (f.isDirectory()) {
                    checkDir(f);
                } else {
                    checkFile(f);
                }
            }
        }
    }

    /**
     * 判断文件底层是否包含某个特定的字符串
     * @param f 文件的File类包装
     * @param s 特定的字符串
     * @return 如果包含 返回true 如果不包含 返回false
     */
    private static boolean fileContainsString(File f,String s){
        byte[] cmp = s.getBytes();
        byte[] fileToByteArray = null;
        try {
            fileToByteArray = FileUtils.readFileToByteArray(f);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return isIncludeByteArray(fileToByteArray,cmp);
    }
    /**
     * 判断是否一个字节数组按顺序包含另一个字节数组
     *
     * @param src
     * @param sub
     * @return 如果包含 返回true 如果不包含 返回false
     */
    private static boolean isIncludeByteArray(byte[] src, byte[] sub) {
        if (src == null || sub == null){
            return false;
        }
        boolean ret = false;
        int srcLen = src.length;
        int subLen = sub.length;

        while(true) {
            if(srcLen < subLen) {
                break;
            }
            int hit = 0;
            for(int i=0; i<srcLen; i++) {
                int tvByteValue = src[i];
                if(hit == sub.length) {
                    ret = true;
                    break;
                }
                if(tvByteValue == sub[hit]) {
                    hit++;
                    continue;
                }
                hit = 0;
                //剩余字节数
                int remain = srcLen - i - 1;
                if(remain < sub.length) {
                    break;
                }
            }
            break;
        }
        return ret;
    }

}
