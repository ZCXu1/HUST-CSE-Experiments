package com.xzc;


import com.xzc.entity.Course;
import com.xzc.entity.SC;
import com.xzc.entity.Student;
import com.xzc.service.CourseService;
import com.xzc.service.SCService;
import com.xzc.service.StudentService;
import com.xzc.service.impl.CourseServiceImpl;
import com.xzc.service.impl.SCServiceImpl;
import com.xzc.service.impl.StudentServiceImpl;
import com.xzc.utils.PrintTable;
import com.xzc.utils.Utils;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

import java.util.*;


@SuppressWarnings("all")
public class Main {
    private static StudentService ss = new StudentServiceImpl();
    private static CourseService cs = new CourseServiceImpl();
    private static SCService scs = new SCServiceImpl();


    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        String op;
        while (true) {
            Utils.showMenu();
            op = sc.nextLine();
            if ("1".equals(op)) {
                System.out.print("请输入学生学号: ");
                String Sno = sc.nextLine();
                System.out.print("请输入学生姓名: ");
                String Sname = sc.nextLine();
                System.out.print("请输入学生性别(男/女): ");
                String Ssex = sc.nextLine();
                System.out.print("请输入学生年龄: ");
                int Sage = sc.nextInt();
                System.out.print("请输入学生所在系: ");
                String Sdept = sc.nextLine();
                //刚入学无奖学金
                ss.addStudent(new Student(Sno, Sname, Ssex, Sage, Sdept, "无"));
                System.out.println("添加成功！");
            } else if ("2".equals(op)) {
                System.out.print("请输入学生学号: ");
                String Sno = sc.nextLine();
                Map<String, Object> map = new HashMap<>();
                map.put("Sno", Sno);
                String Sname = ss.getStudentBySno(Sno).getSname();
                String Ssex = ss.getStudentBySno(Sno).getSsex();
                int Sage = ss.getStudentBySno(Sno).getSage();
                String Sdept = ss.getStudentBySno(Sno).getSdept();
                String Scholarship = ss.getStudentBySno(Sno).getScholarship();
                System.out.print("是否修改学生姓名？(1:Yes/0:No)");
                int i = sc.nextInt();
                if (i == 1) {
                    System.out.print("请输入学生姓名: ");
                    Sname = sc.nextLine();
                }
                System.out.print("是否修改学生性别？(1:Yes/0:No)");
                i = sc.nextInt();
                if (i == 1) {
                    System.out.print("请输入学生性别(男/女): ");
                    Ssex = sc.nextLine();
                }
                System.out.print("是否修改学生年龄？(1:Yes/0:No)");
                i = sc.nextInt();
                if (i == 1) {
                    System.out.print("请输入学生性别年龄: ");
                    Sage = sc.nextInt();
                }
                System.out.print("是否修改学生所在系？(1:Yes/0:No)");
                i = sc.nextInt();
                if (i == 1) {
                    System.out.print("请输入学生所在系: ");
                    Sdept = sc.nextLine();
                }
                System.out.print("是否修改学生奖学金？(1:Yes/0:No)");
                i = sc.nextInt();
                if (i == 1) {
                    System.out.print("请输入学生奖学金(无/具体金额): ");
                    Scholarship = sc.nextLine();
                }
                map.put("Sname", Sname);
                map.put("Ssex", Ssex);
                map.put("Sage", Sage);
                map.put("Sdept", Sdept);
                map.put("Scholarship", Scholarship);
                ss.updateStudent(map);
                System.out.println("修改成功！");
            } else if ("3".equals(op)) {
                System.out.print("请输入课程号: ");
                String Cno = sc.nextLine();
                System.out.print("请输入课程名: ");
                String Cname = sc.nextLine();
                System.out.print("请输入先修课程的课程号(若无，则填0): ");
                String Cpno = sc.nextLine();
                System.out.print("请输入课程学分: ");
                int Ccredit = sc.nextInt();
                if ("0".equals(Cpno)) {
                    cs.addCourse(new Course(Cno, Cname, null, Ccredit));
                } else {
                    cs.addCourse(new Course(Cno, Cname, Cpno, Ccredit));
                }
                System.out.println("添加成功！");
            } else if ("4".equals(op)) {
                System.out.print("请输入课程号: ");
                String Cno = sc.nextLine();
                Map<String, Object> map = new HashMap<>();
                map.put("Cno", Cno);
                String Cname = cs.getCourseByCno(Cno).getCname();
                String Cpno = cs.getCourseByCno(Cno).getCpno();
                int Ccredit = cs.getCourseByCno(Cno).getCcredit();
                System.out.print("是否修改课程姓名？(1:Yes/0:No)");
                int i = sc.nextInt();
                if (i == 1) {
                    System.out.print("请输入课程名: ");
                    Cname = sc.nextLine();
                }
                System.out.print("是否修改课程先修课程？(1:Yes/0:No)");
                i = sc.nextInt();
                if (i == 1) {
                    System.out.print("请输入先修课程的课程号(若无，则填0): ");
                    Cpno = sc.nextLine();
                }
                System.out.print("是否修改课程学分？(1:Yes/0:No)");
                i = sc.nextInt();
                if (i == 1) {
                    System.out.print("请输入课程学分: ");
                    Ccredit = sc.nextInt();
                }
                map.put("Cname", Cname);
                map.put("Ccredit", Ccredit);
                if ("0".equals(Cpno)) {
                    map.put("Cpno", null);
                    cs.updateCourse(map);
                } else {
                    map.put("Cpno", Cpno);
                    cs.updateCourse(map);
                }
                System.out.println("修改成功！");
            } else if ("5".equals(op)) {
                List<Student> students = ss.getStudents();
                List<List<String>> l = Utils.studentsTo2dList(students);
                new PrintTable(l).printTable();
            } else if ("6".equals(op)) {
                System.out.print("请输入学生学号: ");
                String Sno = sc.nextLine();
                System.out.println(ss.getStudentAverageGrade(Sno));
            } else if ("7".equals(op)) {
                System.out.print("请输入学生学号: ");
                String Sno = sc.nextLine();
                System.out.print("请输入课程号: ");
                String Cno = sc.nextLine();
                System.out.println("请输入成绩: ");
                int grade = sc.nextInt();
                scs.addSC(new SC(Sno, Cno, grade));
                System.out.println("添加成功");
            } else if ("8".equals(op)) {
                System.out.print("请输入学生学号: ");
                String Sno = sc.nextLine();
                System.out.print("请输入课程号: ");
                String Cno = sc.nextLine();
                System.out.println("请输入成绩: ");
                int grade = sc.nextInt();
                scs.updateGrade(new SC(Sno, Cno, grade));
                System.out.println("更改成功");
            } else if ("9".equals(op)) {
                System.out.print("请输入课程号: ");
                String Cno = sc.nextLine();
                new PrintTable(ss.getAll(Cno)).printTable();
            } else if ("10".equals(op)) {
                System.out.print("请输入学号：");
                String Sno = sc.nextLine();
                Utils.printInfo(Sno);
                List<String> courses = ss.getCourses(Sno);
                System.out.println("所选课程: " + courses.toString());
            } else if ("11".equals(op)) {
                System.out.print("请输入系名: ");
                String Sdept = sc.nextLine();
                List<Student> students = ss.getStudentsBySdept(Sdept);
                List<SnoAndAvgGrade> list = new ArrayList<>();
                for (Student s : students) {
                    list.add(new SnoAndAvgGrade(s.getSno(), ss.getStudentAverageGrade(s.getSno())));
                }
                Collections.sort(list, (s1, s2) -> {
                    if (new Double(s1.getAvg()).compareTo(new Double(s2.getAvg())) > 0) {
                        return -1;
                    } else {
                        return 1;
                    }
                });
                int count = 1;
                System.out.println("===========================================================================");
                for (SnoAndAvgGrade l : list) {
                    Map<String, String> cag = ss.getCourseAndGrade(l.getSno());
                    System.out.println("第" + (count++) + "名: " + l.getAvg());
                    Utils.printInfo(l.getSno());
                    System.out.println("所选课程及成绩:");
                    for (String key : cag.keySet()) {
                        System.out.print(key + ":" + cag.get(key) + "\t");
                    }
                    System.out.println();
                    System.out.println("===========================================================================");
                }

            } else if ("0".equals(op)) {
                break;
            } else {
                System.out.println("输入错误，请重新输入！");
            }
        }
    }

}

@Data
@NoArgsConstructor
@AllArgsConstructor
class SnoAndAvgGrade {

    private String Sno;

    private String avg;
}
