package com.xzc.service.impl;

import com.xzc.entity.SC;
import com.xzc.entity.Student;
import com.xzc.mapper.StudentMapper;
import com.xzc.service.CourseService;
import com.xzc.service.SCService;
import com.xzc.service.StudentService;
import com.xzc.utils.MyBatisUtil;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import org.apache.ibatis.session.SqlSession;

import java.math.BigDecimal;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@SuppressWarnings("all")
public class StudentServiceImpl implements StudentService {

    SCService scs = new SCServiceImpl();
    CourseService cs = new CourseServiceImpl();

    @Override
    public List<Student> getStudents() {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            StudentMapper mapper = sqlSession.getMapper(StudentMapper.class);
            return mapper.getStudents();
        }
    }

    @Override
    public Student getStudentBySno(String Sno) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            StudentMapper mapper = sqlSession.getMapper(StudentMapper.class);
            return mapper.getStudentBySno(Sno);
        }
    }

    @Override
    public void addStudent(Student student) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            StudentMapper mapper = sqlSession.getMapper(StudentMapper.class);
            mapper.addStudent(student);
            sqlSession.commit();
        }
    }

    @Override
    public void updateStudent(Map<String, Object> map) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            StudentMapper mapper = sqlSession.getMapper(StudentMapper.class);
            mapper.updateStudent(map);
            sqlSession.commit();
        }
    }

    @Override
    public String getStudentAverageGrade(String Sno) {
        SCService scService = new SCServiceImpl();
        List<SC> scs = scService.getSCsBySno(Sno);
        double grade = 0;
        int total = 0;
        for (SC s : scs) {
            if (s.getGrade() != null) {
                grade += s.getGrade();
                total++;
            }
        }
        if (total == 0) {
            return String.valueOf(0.00);
        } else {
            grade /= total;
            DecimalFormat df = new DecimalFormat("0.00");
            String ans = df.format(BigDecimal.valueOf(grade));
            return ans;
        }
    }

    @Override
    public List<Student> getStudentsBySdept(String Sdept) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            StudentMapper mapper = sqlSession.getMapper(StudentMapper.class);
            return mapper.getStudentsBySdept(Sdept);
        }
    }

    @Override
    public List<List<String>> getAll(String Cno) {
        List<List<String>> ans = new ArrayList<>();
        List<String> title = new ArrayList<>();
        title.add("系别");
        title.add("平均成绩");
        title.add("最好成绩");
        title.add("最差成绩");
        title.add("优秀率");
        title.add("不及格人数");
        ans.add(title);

        List<String> snos = scs.getSnosByCno(Cno);
        Map<String, MyList> map = new HashMap<>();
        Map<String, Object> temp = new HashMap<>();
        for (String s : snos) {
            Student student = getStudentBySno(s);
            String Sdept = student.getSdept();
            temp.put("Sno", s);
            temp.put("Cno", Cno);
            int grade = scs.getGradeByCnoAndSno(temp);
            if (map.containsKey(Sdept)) {
                MyList myList = map.get(Sdept);
                int total = myList.getTotal();
                double avg = myList.getAvg();
                int best = myList.getBest();
                int worst = myList.getWorst();
                int countOfGood = myList.getCountOfGood();
                int countOfNotPass = myList.getCountOfNotPass();
                myList.setTotal(total + 1);
                myList.setAvg((avg * total + grade) / (total + 1));
                if (grade > myList.getBest()) {
                    myList.setBest(grade);
                }
                if (grade < myList.getWorst()) {
                    myList.setWorst(grade);
                }
                if (grade >= 80) {
                    myList.setCountOfGood(countOfGood + 1);
                }
                if (grade < 60) {
                    myList.setCountOfNotPass(countOfNotPass + 1);
                }
            } else {
                MyList myList = new MyList(1, grade, grade, grade, 0, 0);
                if (grade >= 80) {
                    myList.setCountOfGood(1);
                }
                if (grade < 60) {
                    myList.setCountOfNotPass(1);
                }
                map.put(Sdept, myList);
            }

        }
        for (String key : map.keySet()) {
            List<String> row = new ArrayList<>();
            MyList myList = map.get(key);
            row.add(key);
            DecimalFormat df = new DecimalFormat("0.00");
            String avg = df.format(BigDecimal.valueOf(myList.getAvg()));
            row.add(avg);
            row.add(String.valueOf(myList.getBest()));
            row.add(String.valueOf(myList.getWorst()));
            String goodpercent = df.format(BigDecimal.valueOf((double) myList.getCountOfGood() / myList.getTotal()));
            row.add(goodpercent);
            row.add(String.valueOf(myList.getCountOfNotPass()));
            ans.add(row);
        }
        return ans;
    }

    @Override
    public List<String> getCourses(String Sno) {
        Student s = getStudentBySno(Sno);
        List<String> cnos = scs.getCnosBySno(Sno);
        List<String> courses = new ArrayList<>();
        for (String st : cnos) {
            courses.add(cs.getCourseByCno(st).getCname());
        }
        return courses;
    }

    @Override
    public Map<String, String> getCourseAndGrade(String Sno) {
        List<SC> SCs = scs.getSCsBySno(Sno);
        Map<String, String> m = new HashMap<>();
        for (SC s : SCs) {
            m.put(cs.getCourseByCno(s.getCno()).getCname(), String.valueOf(s.getGrade()));
        }
        return m;
    }


}

@Data
@NoArgsConstructor
@AllArgsConstructor
class MyList {
    private int total;

    private double avg;

    private int best;

    private int worst;

    private int countOfGood;

    private int countOfNotPass;
}
