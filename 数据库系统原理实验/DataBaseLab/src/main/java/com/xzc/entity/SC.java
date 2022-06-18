package com.xzc.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@SuppressWarnings("all")
@Data
@NoArgsConstructor
@AllArgsConstructor
public class SC {
    /**
     * 学号
     */
    private String Sno;
    /**
     * 课程号
     */
    private String Cno;
    /**
     * 成绩
     */
    private Integer Grade;
}
