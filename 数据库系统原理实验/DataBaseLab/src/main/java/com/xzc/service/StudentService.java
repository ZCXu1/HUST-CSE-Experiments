package com.xzc.service;

import com.xzc.entity.Student;

import java.util.List;
import java.util.Map;

@SuppressWarnings("all")
public interface StudentService {
    /**
     * 返回所有学生
     *
     * @return
     */
    List<Student> getStudents();

    /**
     * 根据学号返回学生
     *
     * @param Sno
     * @return
     */
    Student getStudentBySno(String Sno);

    /**
     * 添加学生信息
     *
     * @param student
     */
    void addStudent(Student student);

    /**
     * 更新学生数据
     *
     * @param map
     */
    void updateStudent(Map<String, Object> map);

    /**
     * 返回某一学生的平均绩点
     *
     * @param Sno
     */
    String getStudentAverageGrade(String Sno);

    /**
     * 返回系里所有的学生
     *
     * @param Sdept
     */
    List<Student> getStudentsBySdept(String Sdept);

    /**
     * 按系统计学生的平均成绩、最好成绩、最差成绩、优秀率、不及格人数。
     *
     * @param Cno
     */
    List<List<String>> getAll(String Cno);

    /**
     * 获取学生所选全部课程
     *
     * @param Sno
     * @return
     */
    List<String> getCourses(String Sno);

    Map<String, String> getCourseAndGrade(String Sno);
}
