package com.xzc.service;

import com.xzc.entity.SC;

import java.util.List;
import java.util.Map;

@SuppressWarnings("all")
public interface SCService {

    List<SC> getSCs();

    void addSC(SC sc);

    void updateGrade(SC sc);

    List<SC> getSCsBySno(String Sno);

    List<String> getSnosByCno(String Cno);

    List<String> getCnosBySno(String Sno);

    int getGradeByCnoAndSno(Map<String, Object> map);
}
