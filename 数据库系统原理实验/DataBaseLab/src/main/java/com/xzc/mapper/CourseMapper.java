package com.xzc.mapper;

import com.xzc.entity.Course;

import java.util.List;
import java.util.Map;

@SuppressWarnings("all")
public interface CourseMapper {

    List<Course> getCourses();

    void addCourse(Course course);

    void updateCourse(Map<String, Object> map);

    void deleteCourseByCno(String Cno);

    Course getCourseByCno(String Cno);

}
