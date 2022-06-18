package com.xzc.mapper;

import com.xzc.entity.Student;

import java.util.List;
import java.util.Map;

@SuppressWarnings("all")
public interface StudentMapper {

    List<Student> getStudents();

    Student getStudentBySno(String Sno);

    void addStudent(Student student);

    void updateStudent(Map<String, Object> map);

    List<Student> getStudentsBySdept(String Sdept);
}
