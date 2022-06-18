package com.xzc.utils;

import org.apache.ibatis.io.Resources;
import org.apache.ibatis.session.SqlSession;
import org.apache.ibatis.session.SqlSessionFactory;
import org.apache.ibatis.session.SqlSessionFactoryBuilder;

import java.io.IOException;
import java.io.InputStream;

@SuppressWarnings("all")
public class MyBatisUtil {
    private static SqlSessionFactory sqlSessionFactory;

    static {
        {
            try {
                String resource = "mybatis-config.xml";
                InputStream inputStream = Resources.getResourceAsStream(resource);
                sqlSessionFactory = new SqlSessionFactoryBuilder().build(inputStream);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    public static SqlSession getSqlSession() {
        return sqlSessionFactory.openSession(false);
    }

}
