package com.blog;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.Date;

import com.google.appengine.api.datastore.Text;
import com.google.appengine.api.users.UserService;
import com.google.appengine.api.users.UserServiceFactory;
import com.google.appengine.api.users.User;
import com.googlecode.objectify.ObjectifyService;

/**
 * Created by Ben on 2/15/2016.
 */
public class PostServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        log("in login servlet");
        String action = request.getParameter("action");
        if (action.equals("Submit")) {
            String title = request.getParameter("title");
            Text content = new Text(request.getParameter("content"));
            log("title: " + title);
            log(content.getValue());
            UserService userService = UserServiceFactory.getUserService();
            User user = userService.getCurrentUser();
            String author = user.getNickname();
            Post post = new Post(author, title, content);
            ObjectifyService.ofy().save().entity(post).now();
            log("saved!");
            request.getRequestDispatcher("index.jsp").forward(request, response);
        }
        else {
            log("cancelled");
            request.getRequestDispatcher("index.jsp").forward(request, response);
        }
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        doPost(request, response);
    }
}
