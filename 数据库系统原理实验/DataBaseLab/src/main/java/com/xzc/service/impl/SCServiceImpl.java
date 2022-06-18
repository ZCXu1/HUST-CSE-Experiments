package com.xzc.service.impl;

import com.xzc.entity.SC;
import com.xzc.mapper.SCMapper;
import com.xzc.service.SCService;
import com.xzc.utils.MyBatisUtil;
import org.apache.ibatis.session.SqlSession;

import java.util.List;
import java.util.Map;

@SuppressWarnings("all")
public class SCServiceImpl implements SCService {

    @Override
    public List<SC> getSCs() {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            SCMapper mapper = sqlSession.getMapper(SCMapper.class);
            return mapper.getSCs();
        }
    }

    @Override
    public void addSC(SC sc) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            SCMapper mapper = sqlSession.getMapper(SCMapper.class);
            mapper.addSC(sc);
            sqlSession.commit();
        }
    }

    @Override
    public void updateGrade(SC sc) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            SCMapper mapper = sqlSession.getMapper(SCMapper.class);
            mapper.updateGrade(sc);
            sqlSession.commit();
        }
    }

    @Override
    public List<SC> getSCsBySno(String Sno) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            SCMapper mapper = sqlSession.getMapper(SCMapper.class);
            return mapper.getSCsBySno(Sno);
        }
    }

    @Override
    public List<String> getSnosByCno(String Cno) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            SCMapper mapper = sqlSession.getMapper(SCMapper.class);
            return mapper.getSnosByCno(Cno);
        }
    }

    @Override
    public List<String> getCnosBySno(String Sno) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            SCMapper mapper = sqlSession.getMapper(SCMapper.class);
            return mapper.getCnosBySno(Sno);
        }
    }

    @Override
    public int getGradeByCnoAndSno(Map<String, Object> map) {
        try (SqlSession sqlSession = MyBatisUtil.getSqlSession()) {
            SCMapper mapper = sqlSession.getMapper(SCMapper.class);
            return mapper.getGradeByCnoAndSno(map);
        }
    }
}
