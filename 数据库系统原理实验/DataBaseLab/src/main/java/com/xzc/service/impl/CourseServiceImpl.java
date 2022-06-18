package com.xzc.service.impl;

import com.xzc.entity.Course;
import com.xzc.mapper.CourseMapper;
import com.xzc.service.CourseService;
import com.xzc.utils.MyBatisUtil;
import org.apache.ibatis.session.SqlSession;

import java.util.List;
import java.util.Map;

@SuppressWarnings("all")
public class CourseServiceImpl implements CourseService {
    @Override
    public List<Course> getCourses() {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            CourseMapper mapper = sqlSession.getMapper(CourseMapper.class);
            return mapper.getCourses();
        }
    }

    @Override
    public void addCourse(Course course) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            CourseMapper mapper = sqlSession.getMapper(CourseMapper.class);
            mapper.addCourse(course);
            sqlSession.commit();
        }
    }

    @Override
    public void updateCourse(Map<String, Object> map) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            CourseMapper mapper = sqlSession.getMapper(CourseMapper.class);
            mapper.updateCourse(map);
            sqlSession.commit();
        }
    }

    @Override
    public void deleteCourseByCno(String Cno) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            CourseMapper mapper = sqlSession.getMapper(CourseMapper.class);
            mapper.deleteCourseByCno(Cno);
            sqlSession.commit();
        }
    }

    @Override
    public Course getCourseByCno(String Cno) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            CourseMapper mapper = sqlSession.getMapper(CourseMapper.class);
            return mapper.getCourseByCno(Cno);
        }
    }
}
