package com.xzc.utils;

import com.xzc.entity.Student;
import com.xzc.service.StudentService;
import com.xzc.service.impl.StudentServiceImpl;

import java.util.ArrayList;
import java.util.List;

@SuppressWarnings("all")
public class Utils {
    private static StudentService ss = new StudentServiceImpl();

    /**
     * 显示菜单
     */
    public static void showMenu() {
        System.out.println("========学生信息管理系统========");
        System.out.println("| 1.添加学生信息  2.修改学生信息 |");
        System.out.println("| 3.添加课程信息  4.修改课程信息 |");
        System.out.println("| 5.查看学生信息  6.查询学生绩点 |");
        System.out.println("| 7.录入学生成绩  8.更改学生成绩 |");
        System.out.println("| 9.按系查询某课程所有的信息     |");
        System.out.println("| 10.查询学生的基本信息和选课信息 |");
        System.out.println("| 11.按系对学生成绩进行排名      |");
        System.out.println("| 0.退出本学生管理系统          |");
        System.out.println("=============================");
        System.out.print("请输入你的选项: ");
    }

    public static void printInfo(String Sno) {
        Student s = ss.getStudentBySno(Sno);
        System.out.print("学号: " + s.getSno());
        System.out.print("\t姓名: " + s.getSname());
        System.out.print("\t性别: " + s.getSsex());
        System.out.print("\t年龄: " + s.getSage());
        System.out.print("\t所在院系: " + s.getSdept());
        System.out.print("\t奖学金: " + s.getScholarship() + "\n");
    }


    public static List<List<String>> studentsTo2dList(List<Student> students) {
        List<List<String>> l = new ArrayList<>();
        List<String> titles = new ArrayList<>();
        titles.add("学号");
        titles.add("姓名");
        titles.add("性别");
        titles.add("年龄");
        titles.add("所在系");
        titles.add("奖学金");
        l.add(titles);
        for (Student s : students) {
            List<String> list = new ArrayList<>();
            list.add(s.getSno());
            list.add(s.getSname());
            list.add(s.getSsex());
            list.add(String.valueOf(s.getSage()));
            list.add(s.getSdept());
            list.add(s.getScholarship());
            l.add(list);
        }
        return l;
    }
}
