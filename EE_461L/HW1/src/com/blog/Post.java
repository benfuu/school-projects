package com.blog; /**
 * Created by Ben on 2/16/2016.
 */

import com.googlecode.objectify.annotation.Entity;
import com.googlecode.objectify.annotation.Id;

import com.google.appengine.api.datastore.Text;
import com.googlecode.objectify.annotation.Index;

import java.util.Date;

@Entity
public class Post {
    @Id public Long id;
    @Index public Date date;
    @Index public String author;
    @Index public String title;
    public Text content;

    public Post() {
        date = new Date();
    }

    public Post(String author, String title, Text content) {
        this();
        this.author = author;
        this.title = title;
        this.content = content;
    }
}
