package com.xzc;

import java.io.File;
import java.util.Scanner;

/**
 * @author XuZichuan
 * @date 2021.6.23
 * @version 1.0.0
 */
public class Main {
    public static void main(String[] args) {
        int op = 0;
        while (true){
            showMenu();
            Scanner sc = new Scanner(System.in);
            op = sc.nextInt();
            if (op != 1 && op != 2){
                System.out.println("输入选项有误！请重新输入。");
            }else{
                break;
            }
        }
        if (op == 1){
            String filePath;
            while (true){
                System.out.print("请输入文件绝对路径：");
                Scanner sc = new Scanner(System.in);
                filePath = sc.nextLine();
                File file = new File(filePath);
                if (!file.exists()){
                    System.out.println("输入路径有误！请重新输入。");
                }else if(file.isDirectory()){
                    System.out.println("输入路径为文件夹路径！请重新输入。");
                }else{
                    break;
                }
            }
            File file = new File(filePath);
            if (file.exists()){
                AntiVirus.checkFile(file);
            }
        }else{
            String dirPath;
            while (true){
                System.out.print("请输入文件夹绝对路径：");
                Scanner sc = new Scanner(System.in);
                dirPath = sc.nextLine();
                File file = new File(dirPath);
                if (!file.exists()){
                    System.out.println("输入路径有误！请重新输入。");
                }else if(!file.isDirectory()){
                    System.out.println("输入路径为文件路径！请重新输入。");
                }else{
                    break;
                }
            }
            File dir = new File(dirPath);
            if (dir.exists()){
                AntiVirus.checkDir(dir);
            }
        }
    }


    private static void showMenu(){
        System.out.println("请选择查杀文件或文件夹:");
        System.out.println("1.查杀文件");
        System.out.println("2.查杀文件夹");
    }
}
